#include "Parser.hpp"

Command Parser::parse(const std::string& line)
{
	Command cmd;
	size_t space_pos = line.find(' ');
	if (space_pos == std::string::npos)
	{
		cmd.name = line;
	}
	else
	{
		cmd.name = line.substr(0, space_pos);
	}
	return cmd;
}
