#include "Dispatcher.hpp"
#include "../rules/commands/Commands.hpp"

Dispatcher::Dispatcher()
{
	_handlers["PASS"] = &cmdPass;
	_handlers["NICK"] = &cmdNick;
	_handlers["USER"] = &cmdUser;
	_handlers["JOIN"] = &cmdJoin;
    _handlers["PRIVMSG"] = &cmdPrivmsg;
    _handlers["TOPIC"] = &cmdTopic;
    _handlers["KICK"] = &cmdKick;
    _handlers["INVITE"] = &cmdInvite;
}

Dispatcher::~Dispatcher(){}

void Dispatcher::dispatch(Client* client, const Command& cmd, ServerState& state)
{
    std::map<std::string, CommandHandler>::iterator it = _handlers.find(cmd.name);

    if (it == _handlers.end())
    {
        reply(client, 421, cmd.name + " :Unknown command");
        return;
    }

    (it->second)(client, cmd, state);
}
