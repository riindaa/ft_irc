#include "Dispatcher.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Command.hpp"
#include <iostream>


Dispatcher::Dispatcher()
{
	_handlers["PASS"] = &Server::handleNick;
	_handlers["NICK"] = &Server::handleNick;
	_handlers["USER"] = &Server::handleNick;
	_handlers["JOIN"] = &Server::handleNick;
	_handlers["PRIVMSG"] = &Server::handleNick;
	_handlers["KICK"] = &Server::handleNick;
	_handlers["INVITE"] = &Server::handleNick;
	_handlers["TOPIC"] = &Server::handleNick;
	_handlers["MODE"] = &Server::handleNick;
}

Dispatcher::~Dispatcher(){}

void Dispatcher::dispatch(Server& server, Client* client, const Command& cmd)
{


	std::map<std::string, CommandHandler>::iterator it;
	it = _handlers.find(cmd.name);
	if (it == _handlers.end())
	{
		// TODO: ERR_UNKNOWNCOMMAND (421)
		std::cout << "Unknown command: " << cmd.name << std::endl;
		return;
	}
	std::cout << "Dispatching: " << cmd.name << std::endl;
	(server.*(it->second))(client, cmd);
}
