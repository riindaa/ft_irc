#include <cctype>

#include "Commands.hpp"

static bool isSpecial(char c)
{
    return std::string("[]\\`_^{|}").find(c) != std::string::npos;
}

static bool isValidNickname(const std::string& nick)
{
    if (nick.empty() || nick.length() > 9)
        return false;

    if (!std::isalpha(static_cast<unsigned char>(nick[0])) && !isSpecial(nick[0]))
        return false;

    for (size_t i = 0; i < nick.length(); ++i)
    {
        if (!std::isalnum(static_cast<unsigned char>(nick[i])) && !isSpecial(nick[i]) &&
            nick[i] != '-')
            return false;
    }
    return true;
}

static void broadcastNickChange(Client* client, const std::string& message)
{
    std::set<Client*> recipients;
    recipients.insert(client);

    const std::set<Channel*>& channels = client->getChannels();
    for (std::set<Channel*>::const_iterator ch = channels.begin(); ch != channels.end(); ++ch)
    {
        const std::map<Client*, bool>& members = (*ch)->getClients();
        for (std::map<Client*, bool>::const_iterator it = members.begin(); it != members.end();
             ++it)
            recipients.insert(it->first);
    }

    for (std::set<Client*>::iterator it = recipients.begin(); it != recipients.end(); ++it)
        (*it)->appendOutBuff(message);
}

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

    if ((foundNickUser && foundNickUser != client) || state.getBot().isNickname(nickname))
        return reply(client, 433, nickname + " :Nickname already used");

    if (nickname == client->getNickname())
        return;

    if (client->isRegistered())
        broadcastNickChange(client, client->getPrefix() + " NICK :" + nickname + "\r\n");

    client->setNickname(nickname);
    state.tryRegister(client);
}
