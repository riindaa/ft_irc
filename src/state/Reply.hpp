#ifndef REPLY_HPP
#define REPLY_HPP

#include <string>

class Client;

#define SERVER_NAME "irc.42.fr"

void reply(Client* client, int code, const std::string& params);

#endif
