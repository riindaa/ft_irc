#ifndef PARSER_HPP
#define PARSER_HPP

#include "Command.hpp"
#include <cstring>

class Parser
{
	public:
		static Command parse(const std::string& line);
};

#endif
