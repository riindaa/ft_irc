#include <iostream>
#include "Dispatcher.hpp"
#include "../rules/commands/Pass.hpp"
#include "../rules/commands/Nick.hpp"
#include "../rules/commands/User.hpp"
#include "../rules/commands/Join.hpp"

Dispatcher::Dispatcher()
{
	_handlers["PASS"] = &cmdPass;
	_handlers["NICK"] = &cmdNick;
	_handlers["USER"] = &cmdUser;
	_handlers["JOIN"] = &cmdJoin;
}

Dispatcher::~Dispatcher(){}

void Dispatcher::dispatch(Client* client, const Command& cmd, ServerState& state)
{
    std::map<std::string, CommandHandler>::iterator it = _handlers.find(cmd.name);

    if (it == _handlers.end())
    {
        std::cout << "Unknown command: " << cmd.name << std::endl;
        return;
    }

    (it->second)(client, cmd, state);
}
