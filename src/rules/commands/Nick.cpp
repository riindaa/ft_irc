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

    const std::string& nickname = cmd.params[0];

    if (!isValidNickname(nickname))
        return reply(client, 432, nickname + " :Erroneous nickname");

    Client* foundNickUser = state.getClientByNick(nickname);

    if (foundNickUser && foundNickUser != client)
        return reply(client, 433, nickname + " :Nickname already used");

    if (nickname == client->getNickname())
        return;

    if (client->isRegistered())
        client->appendOutBuff(":" + client->getNickname() + " NICK :" + nickname + "\r\n");

    client->setNickname(nickname);
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
