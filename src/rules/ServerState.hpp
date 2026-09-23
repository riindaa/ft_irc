#ifndef SERVERSTATE_HPP
#define SERVERSTATE_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include <string>
#include <map>

class Server;

class ServerState {
private:
    std::string _password;
    std::map<int, Client*> _clients;
    std::map<std::string, Channel*> _channels;

public:
    ServerState(const std::string& password);
    ~ServerState();

    Client* getClientByFd(int fd);
    Client* getClientByNick(const std::string& nick);
    Channel* getChannel(const std::string& name);

    void addClient(int fd, Client* client);
    void removeClient(int fd); 
    Channel* createChannel(const std::string& name);
    void removeChannelIfEmpty(const std::string& name);

    bool checkPassword(const std::string& input) const;
};

#endif
