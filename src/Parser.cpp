#include "Parser.hpp"

Command Parser::parse(const std::string& line)
{
	Command cmd;

	std::string buffer = line;

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

	size_t pos;
	while (!buffer.empty())
	{
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

	return cmd;
}
