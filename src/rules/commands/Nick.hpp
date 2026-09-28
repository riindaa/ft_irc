#ifndef NICK_HPP
#define NICK_HPP

#include <string>

#include "Command.hpp"

class Client;
class ServerState;

void cmdNick(Client* client, const Command& cmd, ServerState& state);
bool isValidNickname(const std::string& nick);

#endif
