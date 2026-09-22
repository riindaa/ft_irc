#include "Parser.hpp"

char const *Parser::InvalidMessageException ::what() const throw()
{
	return "Invalid IRC  message";
}

void Parser::skipSpaces(std::string& buffer)
{
	while(!buffer.empty() &&  buffer[0] == ' ')
		buffer.erase(0, 1);
}

void Parser::parsePrefix(std::string& buffer, Command& cmd)
{
	skipSpaces(buffer);
	if (buffer.empty() || buffer[0] != ':')
		return;
	size_t space_pos = buffer.find(' ');

	if(space_pos == std::string::npos || space_pos == 1)
		throw InvalidMessageException();

	cmd.prefix = buffer.substr(1, space_pos - 1);
	buffer.erase(0, space_pos + 1);
}

void Parser::parseCommand(std::string& buffer, Command& cmd)
{
	skipSpaces(buffer);
	if (buffer.empty())
		throw InvalidMessageException();
	size_t space_pos = buffer.find(' ');

	if (space_pos == std::string::npos)
	{
		cmd.name = buffer;
		buffer.clear();
	}
	else
	{
		cmd.name = buffer.substr(0, space_pos);
		buffer.erase(0, space_pos + 1);
	}
}

void Parser::parseParams(std::string& buffer, Command& cmd)
{
	size_t pos;
	while (!buffer.empty())
	{
		skipSpaces(buffer);
		if (buffer.empty())
			break;
		if (buffer[0] == ':')
		{
			std::string trailing = buffer.substr(1);
			cmd.params.push_back(trailing);
			break;
		}

		pos = buffer.find(' ');

		if (pos == std::string::npos)
		{
			cmd.params.push_back(buffer);
			break;
		}
		std::string param = buffer.substr(0, pos);
		cmd.params.push_back(param);
		buffer.erase(0, pos + 1);
	}
}

Command Parser::parse(const std::string& line)
{
	Command cmd;

	std::string buffer = line;
	if (buffer.size() >= 2 && buffer[buffer.size() - 2] == '\r' && buffer[buffer.size() - 1] == '\n')
		buffer.erase(buffer.size() - 2, 2);

	if (buffer.size() > MAX_MESSAGE_LENGTH) // can be deleted
		throw InvalidMessageException();
	parsePrefix(buffer, cmd);
	parseCommand(buffer, cmd);
	parseParams(buffer, cmd);

	return cmd;
}

