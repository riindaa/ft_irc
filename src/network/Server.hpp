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

#include "ServerState.hpp"

class Server {
private:
    int _fd;
    int _port;
    std::string _host;
    std::vector<pollfd> _pollfds;
    
    ServerState _state;

    bool set_non_blocking(int fd);
    bool set_sockopt();
    bool set_bind();
    void create_pfd(int fd);

    void acceptNewConnection();
    void handleClientData(int client_fd);

public:
    Server(int port, const std::string &host, const std::string &password);
    ~Server();

    bool setup();
    void run();
    void closeSocket();

    const std::string &getHost() const;
    int getPort() const;
    int getFd() const;

    void disconnectClient(int fd);
    void updatePollEvents(int client_fd);
    void Server::handleClientWrite(int client_fd);

};

void handle_signal(int sig);

#endif