#ifndef PARSER_HPP
#define PARSER_HPP

#include "Command.hpp"
#include <cstring>

class Parser
{
	private:
		static void parsePrefix(std::string& buffer, Command& cmd);
		static void parseCommand(std::string& buffer, Command& cmd);
		static void parseParams(std::string& buffer, Command& cmd);

	public:
		static Command parse(const std::string& line);
};

#endif
