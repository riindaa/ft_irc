#include "Commands.hpp"

static bool checkErrors(Client* client, Channel* channel, Client* target,
                        const std::string& channelName, const std::string& targetNick)
{
    if (!target)
        return reply(client, 401, targetNick + " :No such nick"), false;

    if (!channel)
        return reply(client, 403, channelName + " :No such channel"), false;

    if (!channel->isMember(client))
        return reply(client, 442, channelName + " :You're not on that channel"), false;

    if (channel->isInviteOnly() && !channel->isOperator(client))
        return reply(client, 482, channelName + " :You're not channel operator"), false;

    if (channel->isMember(target))
        return reply(client, 443, targetNick + " " + channelName + " :is already on channel"),
               false;

    return true;
}

void cmdInvite(Client* client, const Command& cmd, ServerState& state)
{
    if (!client)
        return;

    if (!client->isRegistered())
        return reply(client, 451, ":You have not registered");

    if (cmd.params.size() < 2)
        return reply(client, 461, cmd.name + " :Not enough parameters");

    const std::string& targetNick = cmd.params[0];
    const std::string& channelName = cmd.params[1];
    Channel* channel = state.getChannel(channelName);
    Client* target = state.getClientByNick(targetNick);

    if (!checkErrors(client, channel, target, channelName, targetNick))
        return;

    channel->addInvite(target);
    reply(client, 341, targetNick + " " + channelName);
    target->appendOutBuff(client->getPrefix() + " INVITE " + targetNick + " " + channelName +
                          "\r\n");
}
