#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <poll.h>
#include "Dispatcher.hpp"

class Client;
class Channel;
struct Command;

class Server {
private:
    int                         _port;
    std::string                 _password;
    int                         _serverSocket;
    std::vector<struct pollfd>  _pollFds;

    std::map<int, Client*>      _clients;
    std::map<std::string, Channel*> _channels;
    Dispatcher _dispatcher;

public:
    Server(int port, const std::string& password);
    ~Server();

    void init();
    void run();

    void acceptNewClient();
    void removeClient(int fd);

    void handleClientData(int fd);
    void executeCommand(Client* client, const Command& cmd);

    Client* getClientByNick(const std::string& nick);
    Channel* getOrCreateChannel(const std::string& name);
};

#endif
