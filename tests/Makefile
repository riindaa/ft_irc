NAME		= build/test_commands

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -g
INCLUDES	= -I../src/rules -I../src/rules/commands -I../include

RULES_DIR	= ../src/rules

# Commandes a compiler : toutes par defaut (sauf EXCLUDE), ou une selection
#   make run CMDS="Pass Nick"
EXCLUDE		?= Privmsg
CMDS		?= $(filter-out $(EXCLUDE),$(basename $(notdir $(wildcard $(RULES_DIR)/commands/*.cpp))))

# Toute la logique IRC, sans la partie reseau (Server.cpp, main.cpp)
SRCS		= $(wildcard $(RULES_DIR)/*.cpp) \
			  $(addprefix $(RULES_DIR)/commands/,$(addsuffix .cpp,$(CMDS))) \
			  $(wildcard $(RULES_DIR)/commands/utils/*.cpp) \
			  test_commands.cpp

OBJS		= $(patsubst %.cpp,build/%.o,$(subst ../,,$(SRCS)))
DEPS		= $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

build/%.o: ../%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

run: $(NAME)
	./$(NAME)

valgrind: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all ./$(NAME)

clean:
	rm -rf build

fclean: clean

re: fclean all

-include $(DEPS)

.PHONY: all run valgrind clean fclean re
