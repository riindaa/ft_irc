#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <map>

class Client; // Forward declaration

class Channel {
private:
    std::string             _name;
    std::string             _topic;
    std::string             _key;
    size_t                  _userLimit;
    bool                    _inviteOnly;
    bool                    _topicProtected;

    std::vector<Client*>    _members;
    std::vector<Client*>    _operators;

public:
    Channel(const std::string& name);
    ~Channel();

    const std::string& getName() const;
    const std::string& getTopic() const;
    bool isInviteOnly() const;
    bool isMember(Client* client) const;
    bool isOperator(Client* client) const;

    void setTopic(const std::string& topic);
    void addMember(Client* client);
    void removeMember(Client* client);
    void addOperator(Client* client);
    void removeOperator(Client* client);

    void broadcast(const std::string& message, Client* sender = nullptr);
};

#endif