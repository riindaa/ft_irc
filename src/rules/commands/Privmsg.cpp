#include "Commands.hpp"

void cmdPrivmsg(Client* client, const Command& cmd, ServerState& state)
{
    if (!client)
        return;

    if (cmd.params.empty())
        return reply(client, 411, cmd.name + " :No recipient given (PRIVMSG)");

    if (!client->isRegistered())
        return reply(client, 451, ":You have not registered");

    if (cmd.params.size() < 2 || !cmd.params[1].empty())
        return reply(client, 412, ":No text to send");

    if (!state.getClientByNick(cmd.params[0]))
        return reply(client, 401, cmd.params[0] + " :No such nick/channel");
}
