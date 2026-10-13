NAME = ircserv

CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98
INC = -Iinclude -Isrc/server -Isrc/state -Isrc/commands

SRC = 	src/main.cpp \
		src/server/Server.cpp \
		src/parser/Parser.cpp \
		src/parser/Dispatcher.cpp \
		src/state/Channel.cpp \
		src/state/Client.cpp \
		src/state/Reply.cpp \
		src/state/ServerState.cpp \
		src/commands/Nick.cpp \
		src/commands/Pass.cpp \
		src/commands/User.cpp \
		src/commands/Topic.cpp \
		src/commands/Join.cpp \
		src/commands/Privmsg.cpp \
		src/commands/Kick.cpp \
		src/commands/Invite.cpp \
		src/commands/Mode.cpp \
 		src/commands/utils/Split.cpp \
		src/bot/Bot.cpp \

HDR = 	include/irc.hpp \
		src/server/Server.hpp \
		src/parser/Dispatcher.hpp \
		src/parser/Parser.hpp \
		src/state/Channel.hpp \
		src/state/Client.hpp \
		src/state/Reply.hpp \
		src/state/ServerState.hpp \
		src/commands/Command.hpp \
		src/commands/Commands.hpp \
		src/bot/Bot.hpp \

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

format:
	clang-format -i $(SRC) $(HDR)

check-format:
	clang-format --dry-run --Werror $(SRC) $(HDR)

.PHONY: all clean fclean re format check-format
