#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <poll.h>
#include <csignal>

#include "Channel.hpp"
#include "Client.hpp"

class Server {
private:
    int _fd;
    int _port;
    std::string _host;
    std::vector<struct pollfd> _pollfds;
    
    ServerState _state;

    bool set_non_blocking();
    bool set_sockopt();
    bool set_bind();

    void acceptNewConnection();
    void handleClientData(int fd);

public:
    Server(int port, const std::string &host, const std::string &password);
    ~Server();

    bool setup();
    void run();
    void closeSocket();
};

void handle_signal(int sig);

#endif