#ifndef PASS_HPP
#define PASS_HPP

#include "Command.hpp"

class Client;
class ServerState;

void cmdPass(Client* client, const Command& cmd, ServerState& state);

#endif
