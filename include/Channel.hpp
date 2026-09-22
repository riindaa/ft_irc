#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <set>
#include <string>

class Client;

class Channel
{
  private:
    std::string _name;
    std::string _topic;
    std::string _key;

    std::map<Client *, bool> _clients;
    std::set<Client *> _invitedClients;
    size_t _userLimit;
    bool _inviteOnly;
    bool _topicRestricted;

  public:
    Channel(const std::string &name, const std::string &key = "");
    ~Channel();

    const std::string &getName() const;
    const std::string &getTopic() const;
    const std::string &getKey() const;
    size_t getUserLimit() const;
    bool isInviteOnly() const;
    bool isTopicRestricted() const;

    const std::map<Client *, bool> &getClients() const;
    const std::set<Client *> &getInvitedClients() const;

    bool isMember(Client *client) const;
    bool isOperator(Client *client) const;
    bool isInvited(Client *client) const;

    void setTopic(const std::string &topic);
    void setKey(const std::string &key);
    void setUserLimit(size_t limit);
    void setInviteOnly(bool mode);
    void setTopicRestricted(bool mode);

    void addClient(Client *client, bool isOp = false);
    void removeClient(Client *client);
    void setOperator(Client *client, bool isOp);

    void addInvite(Client *client);
    void removeInvite(Client *client);

    void broadcast(const std::string &message, Client *sender = NULL);
};

#endif
