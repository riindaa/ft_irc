#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <vector>

#include "Command.hpp"
#include "../Client.hpp"
#include "../ServerState.hpp"
#include "../Reply.hpp"

void cmdPass(Client* client, const Command& cmd, ServerState& state);
void cmdNick(Client* client, const Command& cmd, ServerState& state);
void cmdUser(Client* client, const Command& cmd, ServerState& state);
void cmdJoin(Client* client, const Command& cmd, ServerState& state);
void cmdPrivmsg(Client* client, const Command& cmd, ServerState& state);
void cmdKick(Client* client, const Command& cmd, ServerState& state);
void cmdInvite(Client* client, const Command& cmd, ServerState& state);

std::vector<std::string> split(const std::string& str, char sep);

#endif