#include <iomanip>
#include <sstream>

#include "Reply.hpp"
#include "Client.hpp"

void reply(Client* client, int code, const std::string& params)
{
    if (!client)
        return;

    const std::string target =
        client->getNickname().empty() ? "*" : client->getNickname();

    std::ostringstream oss;
    oss << ":" << SERVER_NAME << " "
        << std::setw(3) << std::setfill('0') << code << " "
        << target << " " << params << "\r\n";

    client->appendOutBuff(oss.str());
}
