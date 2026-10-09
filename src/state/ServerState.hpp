#ifndef SERVERSTATE_HPP
#define SERVERSTATE_HPP

#include "../bot/Bot.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include <map>
#include <string>
#include <unistd.h>
#include <vector>

class ServerState
{
  private:
    std::string _password;
    std::map<int, Client*> _clients;
    std::map<std::string, Channel*> _channels;
    Bot _bot;

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
    void removeClientFromAllChannels(Client* client);

    bool checkPassword(const std::string& input) const;

    void cleanUp();
    void tryRegister(Client* client);

    Bot& getBot();
};

#endif
