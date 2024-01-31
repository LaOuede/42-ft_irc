#------------------------------------------------------------------------------#
#                                   COLOR SETTINGS                             #
#------------------------------------------------------------------------------#

# Colors settings
W		:= \033[0m
R		:= \033[1;31m
G		:= \033[1;32m
Y		:= \033[1;33m
C 		:= \033[1;36m


#------------------------------------------------------------------------------#
#                                   TOOLS                                      #
#------------------------------------------------------------------------------#

define HELP
-----------------------------------------------------------------------
$YTools available :$W
make fclean		$Y->$W Suppress executable and archives
make help		$Y->$W Display tools available
make pdf		$Y->$W Open PDF subject
make re			$Y->$W Remove objects and executables and then remake
-----------------------------------------------------------------------
endef
export HELP

#------------------------------------------------------------------------------#
#                                VARIABLES                                     #
#------------------------------------------------------------------------------#

# Executable name
NAME		=	ircserv

# Compiler and flags
CC			=	c++
CPPFLAGS	=	-std=c++98 -Wall -Wextra -Werror
#CPPFLAGS	=	-std=c++98 -Wall -Wextra -Werror -g -fsanitize=address

# Remove
RM			=	rm -rf

# Directories
SRCS_DIR	=	./src
INCS_DIR	=	./inc
OBJS_DIR	=	./obj

# Files
SRCS		=	$(wildcard $(SRCS_DIR)/*.cpp)
OBJS		=	$(patsubst $(SRCS_DIR)/%.cpp,$(OBJS_DIR)/%.o,$(SRCS))
HEADS		=	$(wildcard $(INCS_DIR)/*.hpp) $(wildcard $(INCS_DIR)/*.tpp)

#------------------------------------------------------------------------------#
#                                  RULES                                       #
#------------------------------------------------------------------------------#

# Executable creation
all: dir $(NAME)

# Create directories
dir:
	@mkdir -p $(OBJS_DIR)

# Compilation
$(NAME) : $(OBJS)
	@echo "$(ERASE_LINE)$W\n>>>>>>>>>>>>>>>>>>>> $YCompilation $Wis $Gdone ✅ $W<<<<<<<<<<<<<<<<<<<<"
	@$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME)
	@echo "\n$W---------------------- $(NAME) $Gcreated ✅ $W----------------------\n"

# Compilation of source files
$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp $(HEADS)
	@$(CC) $(CPPFLAGS) -I$(INCS_DIR) -c $< -o $@

# Remove objects and executables
clean:
	@echo "\n$W>>>>>>>>>>>>>>>>>>>>>>>>>>> $YCLEANING $W<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"
	@$(RM) $(OBJS_DIR)
	@echo "$W----------------- $(NAME) : $(OBJS_DIR) was $Rdeleted ❌$W----------------"

fclean: clean
	@$(RM) $(NAME)
	@echo "\n$W--------- All exec. and archives successfully $Rdeleted ❌$W--------\n"
	@echo "$W>>>>>>>>>>>>>>>>>>>>> $YCleaning $Wis $Gdone ✅ $W<<<<<<<<<<<<<<<<<<<<<<\n"

# Display tools available
help:
	@echo "$$HELP"

# Open the subject
pdf:
	@open https://cdn.intra.42.fr/pdf/pdf/94732/fr.subject.pdf

# Remove objects and executables and remake
re: fclean
	@$(MAKE) all

# Start program
run: all
	@echo "\n$W---------------------- $GLaunching $W$(NAME) 🚀 $W----------------------\n"
	./$(NAME) 6667 pass

# Run WeeChat client
wee:
	docker run -it weechat/weechat

# Avoids file-target name conflicts
.PHONY: all clean debug fclean help pdf re rpn wee
