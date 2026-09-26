#include "User.hpp"
#include "../Client.hpp"
#include "../Reply.hpp"
#include "../ServerState.hpp"

void cmdUser(Client* client, const Command& cmd, ServerState& state)
{
    if (!client)
        return;

    if (cmd.params.size() < 4)
        return reply(client, 461, cmd.name + " :Not enough parameters");

    if (client->isRegistered())
        return reply(client, 462, ":You may not reregister");

    client->setUsername(cmd.params[0]);
    client->setRealname(cmd.params[3]);
    state.tryRegister(client);
}
