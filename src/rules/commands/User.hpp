#ifndef USER_HPP
#define USER_HPP

#include "Command.hpp"

class Client;
class ServerState;

void cmdUser(Client* client, const Command& cmd, ServerState& state);

#endif
