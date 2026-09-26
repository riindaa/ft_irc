#include "Join.hpp"
#include "../Client.hpp"
#include "../Reply.hpp"
#include "../ServerState.hpp"

static std::vector<std::string> split(const std::string& str, char sep)
{
    std::vector<std::string> result;
    std::string::size_type start = 0;
    std::string::size_type pos = str.find(sep, start);

    while (pos != std::string::npos)
    {
        result.push_back(str.substr(start, pos - start));
        start = pos + 1;
        pos = str.find(sep, start);
    }
    result.push_back(str.substr(start));
    return result;
}

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

void cmdJoin(Client* client, const Command& cmd, ServerState& state)
{
    (void)state;

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

    for (std::vector<std::string>::size_type i = 0; 
            i < channels.size(); ++i)
    {
        if (!isValidChannelName(channels[i]))
        {
            reply(client, 403, channels[i] + " :No such channel");
            continue;
        }
    }
}
