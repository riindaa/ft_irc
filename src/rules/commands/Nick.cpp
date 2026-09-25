#include <cctype>

#include "Nick.hpp"
#include "../Client.hpp"
#include "../Reply.hpp"
#include "../ServerState.hpp"

void cmdNick(Client* client, const Command& cmd, ServerState& state)
{
    if (!client)
        return;
    
    if (cmd.params.empty())
        return reply(client, 431, ":No nickname given");

    if (!isValidNickname(cmd.params[0]))
        return reply(client, 432, cmd.params[0] + " :Erroneous nickname");

    Client* foundNickUser = state.getClientByNick(cmd.params[0]);

    if (foundNickUser && foundNickUser != client)
        return reply(client, 433, cmd.params[0] + " :Nickname already used");

    client->setNickname(cmd.params[0]);
    state.tryRegister(client);
}

static bool isSpecial(char c)
{
    return std::string("[]\\`_^{|}").find(c) != std::string::npos;
}

bool isValidNickname(const std::string& nick)
{
    if (nick.empty() || nick.length() > 9)
        return false;

    if (!std::isalpha(static_cast<unsigned char>(nick[0])) && !isSpecial(nick[0]))
        return false;

    for (size_t i = 0; i < nick.length(); ++i)
    {
        if (!std::isalnum(static_cast<unsigned char>(nick[i])) && !isSpecial(nick[i]) 
                && nick[i] != '-')
            return false;
    }
    return true;
}
