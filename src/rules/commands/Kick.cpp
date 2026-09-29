#include "Commands.hpp"

static bool checkErrors(Client* client, Channel* channel, Client* target,
                          const std::string& channelName, const std::string& targetNick)
{
    if (!channel)
        return reply(client, 403, channelName + " :No such channel"), false;

    if (!channel->isMember(client))
        return reply(client, 442, channelName + " :You're not on that channel"), false;

    if (!channel->isOperator(client))
        return reply(client, 482, channelName + " :You're not channel operator"), false;

    if (!target || !channel->isMember(target))
        return reply(client, 441, targetNick + " " + channelName + " :They aren't on that channel"), false;

    return true;
}

void cmdKick(Client* client, const Command& cmd, ServerState& state)
{
    if (!client)
        return;

    if (!client->isRegistered())
        return reply(client, 451, ":You have not registered");

    if (cmd.params.size() < 2)
        return reply(client, 461, cmd.name + " :Not enough parameters");

    const std::string& channelName = cmd.params[0];
    const std::string& targetNick = cmd.params[1];
    Channel* channel = state.getChannel(channelName);
    Client* target = state.getClientByNick(targetNick);

    if (!checkErrors(client, channel, target, channelName, targetNick))
        return;

    std::string comment = client->getNickname();
    if (cmd.params.size() > 2 && !cmd.params[2].empty())
        comment = cmd.params[2];

    channel->broadcast(client->getPrefix() + " KICK " + channelName + " " + targetNick + " :" + comment + "\r\n");
    channel->removeClient(target);
    target->removeChannel(channel);
    state.removeChannelIfEmpty(channelName);
}
