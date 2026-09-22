#include "Server.hpp"


void Server::executeCommand(Client* client, const Command& cmd)
{
	_dispatcher.dispatch(*this, client, cmd);
}
