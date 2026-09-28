#ifndef JOIN_HPP
#define JOIN_HPP

#include "Command.hpp"

class Client;
class ServerState;

void cmdJoin(Client* client, const Command& cmd, ServerState& state);

#endif