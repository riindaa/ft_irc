#include "Commands.hpp"

static void sendToChannel(Client* client, const std::string& target,
						  const std::string& message, const std::string& text, ServerState& state)
{

	Channel* channel = state.getChannel(target);
	Bot& bot = state.getBot();

	if (!channel)
		return reply(client, 403, target + " :No such channel");

	if (!channel->isMember(client))
		return reply(client, 404, target + " :Cannot send to channel");

	channel->broadcast(message, client);

	std::string botResponse = bot.handleMessage(text, client->getNickname(), channel);
	if (!botResponse.empty())
	{
		std::string botMessage = bot.getPrefix() + " PRIVMSG " + target + " :" + botResponse + "\r\n";
		channel->broadcast(botMessage);
	}
}

static void sendToClient(Client* client, const std::string& target,
						 const std::string& message, const std::string& text, ServerState& state)
{
	Bot& bot = state.getBot();

	if (bot.isNickname(target))
	{
		std::string botResponse = bot.handleMessage(text, client->getNickname(), NULL);
		if (!botResponse.empty())
		{
			std::string botMessage = bot.getPrefix() + " PRIVMSG " + client->getNickname() + " :" + botResponse + "\r\n";
			client->appendOutBuff(botMessage);
		}
		return;
	}

	Client* recipient = state.getClientByNick(target);

	if (!recipient)
		return reply(client, 401, target + " :No such nick/channel");

	recipient->appendOutBuff(message);
}

void cmdPrivmsg(Client* client, const Command& cmd, ServerState& state)
{
	if (!client)
		return;

	if (!client->isRegistered())
		return reply(client, 451, ":You have not registered");

	if (cmd.params.empty() || cmd.params[0].empty())
		return reply(client, 411, ":No recipient given (" + cmd.name + ")");

	if (cmd.params.size() < 2 || cmd.params[1].empty())
		return reply(client, 412, ":No text to send");

	std::vector<std::string> targets = split(cmd.params[0], ',');

	for (std::vector<std::string>::size_type i = 0; i < targets.size(); ++i)
	{
		const std::string& target = targets[i];

		if (target.empty())
			continue;

		std::string message = client->getPrefix() + " PRIVMSG " + target
			+ " :" + cmd.params[1] + "\r\n";

		if (target[0] == '#')
			sendToChannel(client, target, message, cmd.params[1], state);
		else
			sendToClient(client, target, message, cmd.params[1], state);
	}
}
