#include "Server.hpp"

volatile sig_atomic_t server_running = true;

Server::Server(int port, const std::string &host, const std::string &password)
    : _fd(-1), _port(port), _host(host), _state(password)
{
}

Server::~Server()
{
    closeSocket();
    _state.cleanUp();
}

bool Server::set_non_blocking(int fd)
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "Error fcntl F_SETFL: " << std::strerror(errno) << "\n";
        return false;
    }
    return true;
}

bool Server::set_sockopt()
{
    int opt = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << "Error setsockopt: " << std::strerror(errno) << "\n";
        return false;
    }
    return (true);
}

bool Server::set_bind()
{
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(_host.c_str());
    if (addr.sin_addr.s_addr == INADDR_NONE)
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(_port);

    if (bind(_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        std::cerr << "Error bind\n";
        return false;
    }
    return true;
}

int Server::getFd() const
{
    return _fd;
}

int Server::getPort() const
{
    return _port;
}

const std::string &Server::getHost() const
{
    return _host;
}

void Server::closeSocket()
{
    if (_fd != -1)
        close(_fd);
    _fd = -1;
}

bool Server::setup()
{
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0)
    {
        std::cerr << "Error\n";
        return false;
    }

    if (!set_sockopt())
    {
        closeSocket();
        return false;
    }

    if (!set_non_blocking(_fd))
    {
        closeSocket();
        return false;
    }

    if (!set_bind())
    {
        closeSocket();
        return false;
    }

    if (listen(_fd, SOMAXCONN) < 0)
    {
        std::cerr << "Error listen: " << std::strerror(errno) << "\n";
        closeSocket();
        return false;
    }
    return true;
}

void Server::create_pfd(int fd)
{
    pollfd pfd;
    std::memset(&pfd, 0, sizeof(pfd));
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pollfds.push_back(pfd);
}

void Server::acceptNewConnection()
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0)
    {
        std::cerr << "Error accept " << std::strerror(errno) << "\n";
        return;
    }
    if (!set_non_blocking(client_fd))
    {
        close(client_fd);
        return;
    }
    create_pfd(client_fd);
    Client *client = new Client(client_fd);
    _state.addClient(client_fd, client);
}

void Server::disconnectClient(int fd)
{
    for (std::vector<struct pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it)
    {
        if (it->fd == fd)
        {
            _pollfds.erase(it);
            break;
        }
    }
    Client* client = _state.getClientByFd(fd);
    if (client)
    {
        _state.removeClientFromAllChannels(client);
        _state.removeClient(fd);
    }
    else
        close(fd);
}

void Server::updatePollEvents(int client_fd)
{
    Client* client = _state.getClientByFd(client_fd);
    if (!client)
        return;

    for (size_t i = 0; i < _pollfds.size(); ++i)
    {
        if (_pollfds[i].fd == client_fd)
        {
            if (!client->getOutBuff().empty())
                _pollfds[i].events = POLLIN | POLLOUT;
            else
                _pollfds[i].events = POLLIN;
            break;
        }
    }
}

void Server::handleClientWrite(int client_fd)
{
    Client* client = _state.getClientByFd(client_fd);
    if (!client)
    {
        disconnectClient(client_fd);
        return;
    }
    const std::string& outBuff = client->getOutBuff();
    if (outBuff.empty())
    {
        updatePollEvents(client_fd);
        return;
    }
    ssize_t bytes_sent = send(client_fd, outBuff.c_str(), outBuff.size(), 0);
    if (bytes_sent > 0)
    {
        client->clearOutBuff(bytes_sent);
        updatePollEvents(client_fd);
    }
    else
        disconnectClient(client_fd);
}

void Server::handleClientData(int client_fd)
{
    char buffer[513];
    ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_read <= 0)
    {
        disconnectClient(client_fd);
        return;
    }
    buffer[bytes_read] = '\0';

    Client* client = _state.getClientByFd(client_fd);
    if (!client)
        return;

    client->appendInBuff(buffer);

    std::string command_line;
    while (client->extractNextCommand(command_line))
    {
        //parser
        //executer
    }
    if (client->getInBuff().size() > 512)
        client->clearInBuff(-1);
    updatePollEvents(client_fd);
}

void Server::run()
{
    if (_fd == -1)
        return;
    create_pfd(_fd);
    while (server_running)
    {
        int poll_count = poll(&_pollfds[0], _pollfds.size(), -1);
        if (poll_count < 0)
        {
            if (errno == EINTR)
                continue;
            std::cerr << "Error poll: " << std::strerror(errno) << "\n";
            break;
        }
        for (size_t i = 0; i < _pollfds.size(); ++i)
        {
            int client_fd = _pollfds[i].fd;
            if (_pollfds[i].revents & (POLLHUP | POLLERR))
            {
                disconnectClient(_pollfds[i].fd);
                --i;
                continue;
            }
            else if (_pollfds[i].revents & (POLLIN))
            {
                if (_pollfds[i].fd == _fd)
                {
                    acceptNewConnection();
                    continue;
                }
                else
                {
                    handleClientData(_pollfds[i].fd);
                    if (i < _pollfds.size() && _pollfds[i].fd != client_fd)
                        --i;
                    continue;
                }
            }
            if (i < _pollfds.size() && (_pollfds[i].revents & POLLOUT))
            {
                handleClientWrite(client_fd);
                if (i < _pollfds.size() && _pollfds[i].fd != client_fd)
                    --i;
            }
        }
    }
}

void handle_signal(int sig)
{
    (void) sig;
    server_running = false;
}
