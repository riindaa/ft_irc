#include "Commands.hpp"
#include <cstdlib>


static bool validateMode(Client* client, const Command& cmd, Channel* channel)
{
	if (!channel)
	{
		reply(client, 403, cmd.params[0] + " :No such channel");
		return false;
	}

	if (!channel->isOperator(client))
	{
		reply(client, 482, cmd.params[0] + " :You're not channel operator");
		return false;
	}

	return true;
}


static Client* getModeTarget(Client* client, Channel* channel, ServerState& state, const std::string& nickname)
{
	Client* target = state.getClientByNick(nickname);

	if (!target)
	{
		reply(client, 401, nickname + " :No such nick/channel");
		return NULL;
	}

	if (!channel->isMember(target))
	{
		reply(client, 441, nickname + " " + channel->getName() + " :They aren't on that channel");
		return NULL;
	}

	return target;
}

static bool handleChannelKey(Channel* channel, const Command& cmd, size_t& paramIndex, bool sign)
{
	if (sign)
	{
		if (paramIndex >= cmd.params.size() || cmd.params[paramIndex].empty())
			return false;

		channel->setKey(cmd.params[paramIndex]);
		paramIndex++;
	}
	else
		channel->setKey("");

	return true;
}

static bool handleChannelLimit(Channel* channel, const Command& cmd, size_t& paramIndex, bool sign)
{
	if(sign)
	{
		if (paramIndex  >= cmd.params.size() || cmd.params[paramIndex].empty())
			return false;

		int limit = std::atoi(cmd.params[paramIndex].c_str());

		if (limit > 0)
			channel->setUserLimit(limit);
		paramIndex++;
	}
	else
		channel->setUserLimit(0);

	return true;
}

static void broadcastMode(Client* client, Channel* channel, bool sign, const std::string& mode, const std::string& param = "")
{
	std::string modeSign;

	if (sign)
		modeSign = " +";
	else
		modeSign = " -";

	std::string message = client->getPrefix() + " MODE " + channel->getName() + modeSign + mode;

	if (!param.empty())
		message += " " + param;

	channel->broadcast(message + "\r\n");
}


void cmdMode(Client* client, const Command& cmd, ServerState& state)
{
	bool sign = true;
	bool hasSign = false;

	if (!client)
	return;

	if (!client->isRegistered())
		return reply(client, 451, ":You have not registered");

	if (cmd.params.size() < 2)
		return reply(client, 461, cmd.name + " :Not enough parameters");

	if (cmd.params[1].empty())
		return;

	Channel* channel = state.getChannel(cmd.params[0]);

	if (!validateMode(client, cmd, channel))
		return;

	size_t paramIndex = 2;

	for (size_t i = 0; i < cmd.params[1].size(); i++)
	{
		if (cmd.params[1][i] == '+')
		{
			sign = true;
			hasSign = true;
			continue;
		}

		if (cmd.params[1][i] == '-')
		{
			sign = false;
			hasSign = true;
			continue;
		}
		if (!hasSign)
			continue;

		if (cmd.params[1][i] == 'i')
		{
			channel->setInviteOnly(sign);
			broadcastMode(client, channel, sign, "i");
		}
		else if (cmd.params[1][i] == 't')
		{
			channel->setTopicRestricted(sign);
			broadcastMode(client, channel, sign, "t");
		}

		else if (cmd.params[1][i] == 'k')
		{
			std::string key;

			if (sign && paramIndex < cmd.params.size())
				key = cmd.params[paramIndex];

			if (!handleChannelKey(channel, cmd, paramIndex, sign))
				continue;

			broadcastMode(client, channel, sign, "k", key);
		}
		else if (cmd.params[1][i] == 'o')
		{
			if (paramIndex >= cmd.params.size() || cmd.params[paramIndex].empty())
				continue;

			std::string nickname = cmd.params[paramIndex++];
			Client* target = getModeTarget(client, channel, state, nickname);

			if (!target)
				continue;

			channel->setOperator(target, sign);
			broadcastMode(client, channel, sign, "o", nickname);
		}

		else if (cmd.params[1][i] == 'l')
		{
			std::string limit;

			if (sign && paramIndex < cmd.params.size())
				limit = cmd.params[paramIndex];

			if (!handleChannelLimit(channel, cmd, paramIndex, sign))
				continue;

			broadcastMode(client, channel, sign, "l", limit);
		}

		else
			reply(client, 472, std::string(1, cmd.params[1][i]) + " :is unknown mode char to me");
	}
}
