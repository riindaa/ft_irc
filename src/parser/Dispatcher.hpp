#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include <map>
#include <string>

#include "../commands/Command.hpp"
#include "../state/Client.hpp"
#include "../state/ServerState.hpp"

class Dispatcher
{
  private:
    typedef void (*CommandHandler)(Client*, const Command&, ServerState&);

    std::map<std::string, CommandHandler> _handlers;

  public:
    Dispatcher();
    ~Dispatcher();
    void dispatch(Client* client, const Command& cmd, ServerState& state);
};

#endif
