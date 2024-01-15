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
make fclean		$Y->$W Suppress executable and achives
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

# Files names
SRCS		=	$(wildcard *.cpp)
HEADS		=	$(wildcard *.hpp, *.tpp)

OBJS_DIR	=	./obj/
OBJS_LIST	=	$(patsubst %.cpp, %.o, $(SRCS))
OBJS		=	$(addprefix $(OBJS_DIR), $(OBJS_LIST))

#------------------------------------------------------------------------------#
#                                  RULES                                       #
#------------------------------------------------------------------------------#

# Executable creation
all : dir $(NAME)

#Create directory for *.o files
dir:
	@mkdir -p $(OBJS_DIR)

# Compilation
$(NAME) : $(OBJS)
	@echo "$(ERASE_LINE)$W\n>>>>>>>>>>>>>>>>>>>> $YCompilation $Wis $Gdone ✅ $W<<<<<<<<<<<<<<<<<<<<"
	@$(CC) $(CPPFLAGS) $(SRCS) -o $(NAME)
	@echo "\n$W---------------------- $(NAME) $Gcreated ✅ $W----------------------\n"

# Create all files .o (object) from files .cpp (source code)
$(OBJS_DIR)%.o: %.cpp $(HEADS)
	@$(CC) $(CFLAGS) -c $< -o $@

run: re
	@echo "\n$W---------------------- $GLaunching $W$(NAME) 🚀 $W----------------------\n"
	./$(NAME) 6667 allo

# Remove objects and executables
clean:
	@echo "\n$W>>>>>>>>>>>>>>>>>>>>>>>>>>> $YCLEANING $W<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"
	@$(RM) $(OBJS_DIR)
	@echo "$W----------------- $(NAME) : $(OBJS_DIR) was $Rdeleted ❌$W---------------"

fclean: clean
	@$(RM) $(NAME)
	@echo "\n$W--------- All exec. and archives successfully $Rdeleted ❌$W--------\n"
	@echo "$W>>>>>>>>>>>>>>>>>>>>> $YCleaning $Wis $Gdone ✅ $W<<<<<<<<<<<<<<<<<<<<<<\n"

wee:
	docker run -it weechat/weechat

# Display tools available
help:
	@echo "$$HELP"

# Open the subject
pdf:
	@open https://cdn.intra.42.fr/pdf/pdf/94732/fr.subject.pdf

# Remove objects and executables and remake
re: fclean
	@$(MAKE) all


# Avoids file-target name conflicts
.PHONY: all clean debug fclean help pdf re rpn test