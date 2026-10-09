#include "Commands.hpp"

static bool isValidChannelName(const std::string& name)
{
    if (name.length() < 2 || name.length() > 50)
        return false;

    if (name[0] != '#')
        return false;

    for (size_t i = 1; i < name.length(); ++i)
    {
        if (name[i] == ' ' || name[i] == ',' || name[i] == ':' || name[i] == '\x07')
            return false;
    }
    return true;
}

static void sendMembersToNewClient(Channel* channel, Client* client)
{
    std::string names;
    const std::map<Client*, bool>& members = channel->getClients();

    for (std::map<Client*, bool>::const_iterator it = members.begin();
            it != members.end(); ++it)
    {
        if (!names.empty())
            names += " ";
        if (it->second)
            names += "@";
        names += it->first->getNickname();
    }

    reply(client, 353, "= " + channel->getName() + " :" + names);
    reply(client, 366, channel->getName() + " :End of /NAMES list");
}

static void joinChannel(Client* client, const std::string& name, ServerState& state, 
                            const std::string& key)
{
    Channel* channel = state.getChannel(name);
    bool isNew = (channel == NULL);

    if (isNew)
        channel = state.createChannel(name);

    if (channel->isMember(client))
        return;

    if (channel->isInviteOnly() && !channel->isInvited(client))
    {
        reply(client, 473, name + " :Cannot join channel (+i)");
        return;
    }

    if (!channel->getKey().empty() && (channel->getKey() != key))
    {
        reply(client, 475, name + " :Cannot join channel (+k)");
        return;
    }

    if ((channel->getUserLimit() > 0) && (channel->getClients().size() >= channel->getUserLimit()))
    {
        reply(client, 471, name + " :Cannot join channel (+l)");
        return;
    }

    channel->addClient(client, isNew);
    client->addChannel(channel);
    channel->broadcast(client->getPrefix() + " JOIN " + name + "\r\n");

    if (!channel->getTopic().empty())
        reply(client, 332, name + " :" + channel->getTopic());

    sendMembersToNewClient(channel, client);
}

void cmdJoin(Client* client, const Command& cmd, ServerState& state)
{
    if (!client)
        return;

    if (!client->isRegistered())
        return reply(client, 451, ":You have not registered");

    if (cmd.params.empty())
        return reply(client, 461, cmd.name + " :Not enough parameters");

    std::vector<std::string> channels = split(cmd.params[0], ',');
    std::vector<std::string> keys;

    if (cmd.params.size() > 1)
        keys = split(cmd.params[1], ',');

    for (std::vector<std::string>::size_type i = 0; i < channels.size(); ++i)
    {
        if (!isValidChannelName(channels[i]))
        {
            reply(client, 403, channels[i] + " :No such channel");
            continue;
        }
        std::string key = i < keys.size() ? keys[i] : "";
        joinChannel(client, channels[i], state, key);
    }
}
