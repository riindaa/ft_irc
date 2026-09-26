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
    }
    result.push_back(str.substr(start));
    return result;
}

void cmdJoin(Client* client, const Command& cmd, ServerState& state)
{
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
}
