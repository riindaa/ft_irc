#include "Bot.hpp"
#include <cctype>
#include "../state/Channel.hpp"
#include <sstream>

Bot::Bot() : _nickname("bot") {}

Bot::~Bot() {}

bool Bot::isNickname(const std::string& nickname) const
{
	if (nickname.length() != _nickname.length())
		return false;

	for (size_t i = 0; i < nickname.length(); i++)
	{
		if (std::tolower(static_cast<unsigned char>(nickname[i]))
			!= std::tolower(static_cast<unsigned char>(_nickname[i])))
			return false;
	}

	return true;
}

std::string Bot::handleMessage(const std::string& message, const std::string& nickname, Channel* channel) const
{
	if (message == "!hello")
		return "Hello " + nickname;

	if (message == "!help")
		return "Commands: !hello, !help, !users";

	if (message == "!users" && channel)
	{
		std::ostringstream count;
		count << channel->getClientCount();

		return "Users in " + channel->getName() + " (" + count.str() + "): " + channel->getClientNames();
	}

	return "";
}

const std::string Bot::getPrefix() const
{
	return ":" + _nickname + "!bot@localhost";
}

