#include "Server.hpp"

volatile __sig_atomic_t server_running = true;

Server::Server(int port, const std::string &host, const std::string &password)
    : _host(host), _port(port), _fd(-1), _password(password)
{
}

Server::~Server()
{
}

bool Server::set_non_blocking()
{
    int flags = fcntl(_fd, F_GETFL, 0);
    if (flags < 0)
    {
        std::cerr << "Error fcntl F_GETFL: " << std::strerror(errno) << "\n";
        closeSocket();
        return false;
    }
    if (fcntl(_fd, F_SETFL, flags | O_NONBLOCK) < 0)
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

const std::map<int, Client *> &Server::getClients() const
{
    return _clients;
}

const std::map<std::string, Channel *> &Server::getChannels() const
{
    return _channels;
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

    if (!set_non_blocking())
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

void Server::setClient(Client *newClient)
{
    _clients.insert(std::make_pair(newClient->getFd(), newClient));
}

void Server::setChannel(Channel *newChannel)
{
    _channels.insert(std::make_pair(newChannel->getName(), newChannel));
}

void Server::run()
{
    if (_fd == -1)
        return;
    pollfd server_pfd;

    server_pfd.fd = _fd;
    server_pfd.events = POLLIN;
    server_pfd.revents = 0;
    _pollfds.push_back(server_pfd);

    while (server_running)
    {
        
    }
}

void handle_signal(int sig)
{
    (void) sig;
    server_running = false;
}
