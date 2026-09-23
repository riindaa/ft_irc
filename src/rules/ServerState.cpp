#include <iostream>
#include "ServerState.hpp"

ServerState::ServerState(const std::string& password) : _password(password)
{
}
ServerState::~ServerState()
{
}

Client* ServerState::getClientByFd(int fd)
{
    std::map<int, Client*>::iterator it = _clients.find(fd);

    if (it == _clients.end())
        return NULL;

    return it->second;
}
Client* ServerState::getClientByNick(const std::string& nick)
{
    for (std::map<int, Client*>::iterator it = _clients.begin();
         it != _clients.end(); ++it)
    {
        if (it->second->getNickname() == nick)
            return it->second;
    }
    return NULL;

}
Channel* ServerState::getChannel(const std::string& name)
{
    for (std::map<std::string, Channel*>::iterator it = _channels.begin();
            it != _channels.end(); ++it)
    {
        if (it->second->getName() == name)
            return it->second;
    }
    return NULL;
}

void ServerState::addClient(int fd, Client* client)
{
    if (!_clients.insert(std::make_pair(fd, client)).second)
        throw std::runtime_error("Error: client already exists");
}

void ServerState::removeClient(int fd)
{
    if (_clients.erase(fd) == 0)
        throw std::runtime_error("Error: client not found");
}

Channel* ServerState::createChannel(const std::string& name)
{
    if (_channels.find(name) != _channels.end())
        throw std::runtime_error("Error: channel already exists");

    Channel* channel = new Channel(name);
    _channels.insert(std::make_pair(name, channel));

    return channel;
}
void ServerState::removeChannelIfEmpty(const std::string& name)
{
    std::map<std::string, Channel*>::iterator it = _channels.find(name);

    if (it == _channels.end())
        return;

    if (it->second->isEmpty())
    {
        delete it->second;
        _channels.erase(it);
    }
}

bool ServerState::checkPassword(const std::string& input) const
{
    
}
