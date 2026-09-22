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

class Server
{
  private:
    int _fd;
    int _port;
    std::string _password;
    std::string _host;
    std::map<int, Client *> _clients;
    std::map<std::string, Channel *> _channels;
    std::vector<struct pollfd> _pollfds;

    bool set_non_blocking();
    bool set_sockopt();
    bool set_bind();

  public:
    Server(int port, const std::string &host, const std::string &password);
    ~Server();

    bool setup();
    void run();

    int getFd() const;
    int getPort() const;
    const std::map<int, Client *> &getClients() const;
    const std::map<std::string, Channel *> &getChannels() const;
    const std::string &getHost() const;

    void setChannel(Channel *newChannel);
    void setClient(Client *newClient);


    void closeSocket();
};

void handle_signal(int sig);

#endif