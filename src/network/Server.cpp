#include "Server.hpp"

volatile __sig_atomic_t server_running = true;

Server::Server(int port, const std::string &host, const std::string &password)
    : _host(host), _port(port), _fd(-1), _state(password)
{
}

Server::~Server()
{
}

bool Server::set_non_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
    {
        std::cerr << "Error fcntl F_GETFL: " << std::strerror(errno) << "\n";
        closeSocket();
        return false;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        std::cerr << "Error fcntl F_SETFL: " << std::strerror(errno) << "\n";
        closeSocket();
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
        closeSocket();
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
        closeSocket();
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
        return false;

    if (!set_non_blocking(_fd))
        return false;

    if (!set_bind())
        return false;

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
    pfd.fd = _fd;
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
        std::cerr << "Error accept " << std::strerror(errno) << "\n";
    if (!set_non_blocking(client_fd))
        close(client_fd);
    create_pfd(client_fd);
}

void Server::disconnectClient(int fd)
{
    close(fd);
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
        client->clearInBuff();
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
            if (_pollfds[i].revents & POLLIN)
            {
                if (_pollfds[i].fd == _fd)
                    acceptNewConnection();
                else
                    handleClientData(_pollfds[i].fd);
            }
        }
    }
}

void handle_signal(int sig)
{
    (void) sig;
    server_running = false;
}
