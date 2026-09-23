#include "ServerState.hpp"

ServerState::ServerState(const std::string& password)
{

}
ServerState::~ServerState()
{

}

Client* ServerState::getClientByFd(int fd)
{

}
Client* ServerState::getClientByNick(const std::string& nick)
{

}
Channel* ServerState::getChannel(const std::string& name)
{

}

void ServerState::addClient(int fd, Client* client)
{

}

void ServerState::removeClient(int fd)
{

}

Channel* ServerState::createChannel(const std::string& name)
{

}
void ServerState::removeChannelIfEmpty(const std::string& name)
{

}

bool checkPassword(const std::string& input)
{

}