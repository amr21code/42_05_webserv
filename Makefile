NAME = webserv
SRC_DIR = src/
######## add subdirs here
#BUILTIN_DIR = src/builtins/
########
OBJ_DIR = obj/
INC_DIR = inc/
OS = $(shell uname)
CC = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98
GREEN = \033[0;32m
RED = \033[0;31m
UNDO_COL = \033[0m


# place all source files here
SRC = 	$(SRC_DIR)main.cpp \
		$(SRC_DIR)httpServer.cpp \
		

# takes all named source files and converts them to .o files in the /obj directory
OBJ = $(SRC:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)

# prevents rules from being considered as files
.PHONY: all clean fclean re

all: $(NAME)

# creates subdirectory /obj and subdirectories
$(OBJ): $(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# compiles all object files and builds executable file 'webserv'
$(NAME): $(OBJ)
	@echo "$(RED)Compiling $(NAME)...$(UNDO_COL)"
	$(CC) $(CFLAGS) $^ -o $@
	@echo "$(GREEN)SUCCESSFULLY CREATED WEBSERV!$(UNDO_COL)"

# removes all object files and the /obj directory and cleans libft
clean:
	$(MAKE) clean -C $(LIBFT_DIR)
	rm -r $(OBJ_DIR)
	@echo "$(RED)Finished clean$(UNDO_COL)"

# calls 'clean' and removes the executable
fclean: clean
	$(MAKE) fclean -C $(LIBFT_DIR)
	rm -f $(NAME)
	@echo "$(RED)Finished fclean$(UNDO_COL)"

# re-makes the whole compilation from scratch
re: fclean all

dall: $(OBJ)
	@echo "$(RED)Compiling Debug 3 $(NAME)...$(UNDO_COL)"
	$(CC) $(CFLAGS) -D DEBUG=3 $^ -o $@
	@echo "$(GREEN)SUCCESSFULLY CREATED WEBSERV!$(UNDO_COL)"

dspec: $(OBJ)
	@echo "$(RED)Compiling Debug 1 $(NAME)...$(UNDO_COL)"
	$(CC) $(CFLAGS) -D DEBUG=1 $^ -o $@
	@echo "$(GREEN)SUCCESSFULLY CREATED WEBSERV!$(UNDO_COL)"
 