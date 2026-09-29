#include "Commands.hpp"

void cmdTopic(Client* client, const Command& cmd, ServerState& state)
{
    if (!client)
        return;

    if (!client->isRegistered())
        return reply(client, 451, ":You have not registered");

    if (cmd.params.empty())
        return reply(client, 461, cmd.name + " :Not enough parameters");

    Channel* channel = state.getChannel(cmd.params[0]);

    if (!channel)
        return reply(client, 403, cmd.params[0] + " :No such channel");

    if (!channel->isMember(client))
        return reply(client, 442, channel->getName() + " :You're not on that channel");

    if (cmd.params.size() == 1)
    {
        if (channel->getTopic().empty())
            return reply(client, 331, channel->getName() + " :No topic is set");
        return reply(client, 332, channel->getName() + " :" + channel->getTopic());
    }

    if (channel->isTopicRestricted() && !channel->isOperator(client))
        return reply(client, 482, channel->getName() + " :You're not channel operator");
        
    channel->setTopic(cmd.params[1]);
    channel->broadcast(client->getPrefix() + " TOPIC " + channel->getName() + " :" + cmd.params[1] + "\r\n");
}
