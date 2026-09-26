#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel(const std::string& name, const std::string& key)
    : _name(name), _topic(""), _key(key), _userLimit(0), _inviteOnly(false), _topicRestricted(false)
{
}

Channel::~Channel()
{
}


const std::string& Channel::getName() const
{
    return _name;
}

const std::string& Channel::getTopic() const
{
    return _topic;
}

const std::string& Channel::getKey() const
{
    return _key;
}

size_t Channel::getUserLimit() const
{
    return _userLimit;
}

bool Channel::isInviteOnly() const
{
    return _inviteOnly;
}

bool Channel::isTopicRestricted() const
{
    return _topicRestricted;
}

bool Channel::isEmpty() const
{
   return _clients.empty();
}

const std::map<Client*, bool>& Channel::getClients() const
{
    return _clients;
}

const std::set<Client*>& Channel::getInvitedClients() const
{
    return _invitedClients;
}

bool Channel::isMember(Client* client) const
{
    if (!client)
        return false;
    return _clients.find(client) != _clients.end();
}

bool Channel::isOperator(Client* client) const
{
    if (!client)
        return false;
    std::map<Client*, bool>::const_iterator it = _clients.find(client);
    if (it != _clients.end())
        return it->second;
    return false; 
}

bool Channel::isInvited(Client* client) const
{
    if (!client)
        return false;
    return _invitedClients.count(client) > 0;
}

void Channel::setTopic(const std::string& topic)
{
    _topic = topic;
}

void Channel::setKey(const std::string& key)
{
    _key = key;
}

void Channel::setUserLimit(size_t limit)
{
    _userLimit = limit;
}

void Channel::setInviteOnly(bool mode)
{
    _inviteOnly = mode;
}

void Channel::setTopicRestricted(bool mode)
{
    _topicRestricted = mode;
}

void Channel::addClient(Client* client, bool isOp)
{
    if (client)
    {
        _clients[client] = isOp;
        removeInvite(client);
    }
}

void Channel::removeClient(Client* client)
{
    if (client)
        _clients.erase(client);
}

void Channel::setOperator(Client* client, bool isOp)
{
    if (isMember(client))
        _clients[client] = isOp;
}

void Channel::addInvite(Client* client)
{
    if (client)
        _invitedClients.insert(client);
}

void Channel::removeInvite(Client* client)
{
    if (client)
        _invitedClients.erase(client);
}


void Channel::broadcast(const std::string& message, Client* sender)
{
    for (std::map<Client*, bool>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            Client* temp = it->first;

            if (sender != NULL && temp == sender)
                continue;

            temp->appendOutBuff(message);
        }
}