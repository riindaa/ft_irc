#include "../include/irc.hpp"

int main(int ac, char**av)
{
    (void)av;

    if (ac != 3)
        return 1;

    std::signal(SIGINT, handle_signal);
    std::signal(SIGQUIT, handle_signal);
    std::signal(SIGPIPE, SIG_IGN);

    int port = atoi(av[1]);
    if (!port)
    {
        std::cerr << "Error: bad port\n";
        return 1;
    }

    Server server(port, "0.0.0.0", av[2]);

    if (!server.setup())
        return 1;
    server.run();

    return 0;
}
