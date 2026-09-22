#ifndef PARSER_HPP
#define PARSER_HPP

#include "Command.hpp"
#include <string>
#include <exception>

class Parser
{
	private:
		static void parsePrefix(std::string& buffer, Command& cmd);
		static void parseCommand(std::string& buffer, Command& cmd);
		static void parseParams(std::string& buffer, Command& cmd);
		static void skipSpaces(std::string& buffer);

	public:
		static Command parse(const std::string& line);

		class InvalidMessageException  : public std::exception
		{
			public:
				virtual char const *what() const throw();
		};
};

#endif
