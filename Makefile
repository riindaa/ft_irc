NAME = ircserv

CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98
INC = -Iinclude -Isrc/network -Isrc/rules -Isrc/rules/commands

SRC = 	src/main.cpp \
		src/network/Server.cpp \
		src/parser/Parser.cpp \
		src/parser/Dispatcher.cpp \
		src/rules/Channel.cpp \
		src/rules/Client.cpp \
		src/rules/Reply.cpp \
		src/rules/ServerState.cpp \
		src/rules/commands/Nick.cpp \
		src/rules/commands/Pass.cpp \
		src/rules/commands/User.cpp \
		src/rules/commands/Topic.cpp \
		src/rules/commands/Join.cpp \
		src/rules/commands/Privmsg.cpp \
		src/rules/commands/Kick.cpp \
		src/rules/commands/Invite.cpp \
 		src/rules/commands/utils/Split.cpp \

HDR = 	include/irc.hpp \
		src/network/Server.hpp \
		src/parser/Dispatcher.hpp \
		src/parser/Parser.hpp \
		src/rules/Channel.hpp \
		src/rules/Client.hpp \
		src/rules/Reply.hpp \
		src/rules/ServerState.hpp \
		src/rules/commands/Command.hpp \
		src/rules/commands/Commands.hpp \

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CPP) $(CPPFLAGS) -o $(NAME) $(OBJ)

%.o: %.cpp $(HDR)
	$(CPP) $(CPPFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
