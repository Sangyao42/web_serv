################################
######     Variables     #######
################################

CC:=g++
CXXFLAGS= -std=c++14 -pedantic -Wall -Wextra -Werror -MMD -MP -O2
LDFLAGS= -std=c++14 -pedantic
ifdef FSANITIZE
	CXXFLAGS+= -g3 -fsanitize=address -DDEBUG=1
	LDFLAGS+= -g3 -fsanitize=address
endif

###############################
######     Settings     #######
###############################

NAME:= debug.out
# This specify where the header files are located
INCLUDE_DIR:= ../src .
# This specify where the source files are located
SRC_DIR:= .
# This specify where the object files will be located
OBJS_DIR:= obj

###################################
######     Source files     #######
###################################

# To add souce files, create a varaible for each folder, and then
# contatenate them in the SRC variable like this:

SRC:= $(shell find . \( -name \*.h -o -name \*.cpp \) -print)

####################################
######     Library files     #######
####################################

SYSTEM:= $(shell uname -s)

# To compile a library, store a variable for their library file like this
# and add a rule for it after the main rules:
# LIBRARY_NAME=lib/LIBRARY_NAME/LIBRARY_NAME.a
LIBWEBSERV=../libwebserv.a

# To add a library, add the library header file like this:
# INCLUDE_DIR+= lib/LIBRARY_NAME/include
ifeq ($(SYSTEM),Linux)
	LDFLAGS+= -lgtest -lgtest_main

else ifeq ($(SYSTEM),Darwin)
	LIB_INCLUDE_DIR += $(shell brew --prefix googletest)/include
	LDFLAGS+= -L$(shell brew --prefix googletest)/lib -lgtest  -L$(shell brew --prefix googletest)/lib -lgtest_main
endif

# Then add the library to the linking process in one of the following ways:
# LDFLAGS+= -Llib/LIBRARY_NAME -lLIBRARY_NAME
# LDFLAGS+= lib/LIBRARY_NAME/libLIBRARY_NAME.a
# the archive also includes a main function, which should be overridden by the gtest_main library. So the order of the libraries is important
LDFLAGS+= -L.. -lwebserv

###########################################
######     Object name reformat     #######
###########################################

# This in effect makes all the object files to be compiled in the OBJS_DIR directory
OBJ:=$(addprefix $(OBJS_DIR)/,$(subst /,@,$(SRC:.cpp=.o)))
DEPENDS:=$(OBJ:.o=.d)

#################################
######     Main rules     #######
#################################

all:
	@$(MAKE) $(NAME) -j

$(NAME): $(LIBWEBSERV) $(OBJ)
	@$(CC) $(OBJ) -o $(NAME) $(LDFLAGS) && echo "Compilation of $(NAME) successful"

##########################################
######     Library compilation     #######
##########################################

# To compile a library, add a rule like this:

# $(LIBRARY_NAME):
# 	@${MAKE} $(if $(FSANITIZE),FSANITIZE=yes,) -C lib/LIBRARY_NAME

$(LIBWEBSERV):
	@$(MAKE) $(if $(FSANITIZE),FSANITIZE=yes,) -C .. libwebserv.a

#########################################
######     Object compilation     #######
#########################################

-include $(DEPENDS)

.SECONDEXPANSION:
$(OBJS_DIR)/%.o: $(SRC_DIR)/$$(subst @,/,$$*).cpp
	@$(CC) $(CXXFLAGS) $(addprefix -iquote ,$(INCLUDE_DIR)) $(addprefix -I ,$(LIB_INCLUDE_DIR)) -c $< -o $@

$(OBJ): $(OBJS_DIR)

$(OBJS_DIR):
	@mkdir -p $(OBJS_DIR)

###############################
######     Cleaning     #######
###############################

clean: 
	@rm -f $(OBJ)

fclean: clean
	@rm -rf $(OBJS_DIR)
	@rm -f $(NAME)
	@rm -f $(LIBWEBSERV)

re: fclean all

.PHONY: clean fclean re