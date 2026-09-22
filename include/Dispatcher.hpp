#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include <map>
#include <string>

class Server;
class Client;
struct Command;


class Dispatcher
{
	private:
		typedef void (Server::*CommandHandler)(Client*, const Command&);

		std::map<std::string, CommandHandler> _handlers;

	public:
		Dispatcher();
		~Dispatcher();
		void dispatch(Server& server, Client* client, const Command& cmd);


};

#endif
