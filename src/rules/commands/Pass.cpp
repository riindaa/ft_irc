#include "Pass.hpp"
#include "../Client.hpp"
#include "../Reply.hpp"
#include "../ServerState.hpp"

void cmdPass(Client* client, const Command& cmd, ServerState& state)
{
    if (!client)
        return;
    
    if (cmd.params.empty())
        return reply(client, 461, cmd.name + " :Not enough parameters");

    if (client->isRegistered())
        return reply(client, 462, ":You may not reregister");

    if (!state.checkPassword(cmd.params[0]))
        return reply(client, 464, ":Password incorrect");

    client->setHasPass(true);
    state.tryRegister(client);
}
