#include "../include/irc.hpp"

int main(int ac, char**av)
{
    (void)av;

    if (ac != 3)
        return 1;

    std::signal(SIGINT, handle_signal);
    std::signal(SIGQUIT, handle_signal);
    std:;signal(SIGPIPE, SIG_IGN);

    Server server(8080, "localhost", "password");

    if (!server.setup())
    {
        server.~Server();
        return 1;
    }
    server.run();

    server.~Server();
    return 0;
}
