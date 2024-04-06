################################
######     Variables     #######
################################

CC:=g++
CXXFLAGS= -std=c++98 -pedantic -Wall -Wextra -Werror -MMD -MP
LDFLAGS= -std=c++98 -pedantic
ifdef FSANITIZE
	CXXFLAGS+= -g3 -fsanitize=address -DDEBUG=1
	LDFLAGS+= -g3 -fsanitize=address
else
	CXXFLAGS+= -O2 -DNDEBUG
endif

###############################
######     Settings     #######
###############################

NAME:= webserv
# This specify where the header files are located
INCLUDE_DIR:= src
# This specify where the source files are located
SRC_DIR:= src
# This specify where the object files will be located
OBJS_DIR:= obj

###################################
######     Source files     #######
###################################

# To add souce files, create a varaible for each folder, and then
# contatenate them in the SRC variable like this:

MAIN_SRC:= \
	main.cpp \
	constants.cpp

URI_SRC:= \
	Uri/Authority.cpp

CONFIGURATION_SRC:= \
	Configuration.cpp \
	Configuration/Directive.cpp \
	Configuration/Cache/LocationQuery.cpp \
	Configuration/Cache/ServerQuery.cpp \
	Configuration/Directive/Socket.cpp \
	Configuration/Directive/Block.cpp \
	Configuration/Directive/Block/Main.cpp \
	Configuration/Directive/Block/Http.cpp \
	Configuration/Directive/Block/Server.cpp \
	Configuration/Directive/Block/Events.cpp \
	Configuration/Directive/Block/Location.cpp \
	Configuration/Directive/Simple/Listen.cpp \
	Configuration/Directive/Simple/ServerName.cpp \
	Configuration/Directive/Simple/ErrorPage.cpp \
	Configuration/Directive/Simple/AllowMethods.cpp \
	Configuration/Directive/Simple/MimeTypes.cpp \
	Configuration/Directive/Simple/Return.cpp \
	Configuration/Directive/Simple/Cgi.cpp

MISC_SRC:= \
	misc/Nothing.cpp

SRC:= $(MAIN_SRC) $(URI_SRC) $(CONFIGURATION_SRC) $(MISC_SRC)

####################################
######     Library files     #######
####################################

# To compile a library, store a variable for their library file like this
# and add a rule for it after the main rules:
# LIBRARY_NAME=lib/LIBRARY_NAME/LIBRARY_NAME.a

# To add a library, add the library header file like this:
# INCLUDE_DIR+= lib/LIBRARY_NAME/include

# Then add the library to the linking process in one of the following ways:
# LDFLAGS+= -Llib/LIBRARY_NAME -lLIBRARY_NAME
# LDFLAGS+= lib/LIBRARY_NAME/libLIBRARY_NAME.a

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

$(NAME): $(OBJ)
	@$(CC) $(OBJ) -o $(NAME) $(LDFLAGS) && echo "Compilation of $(NAME) successful"

##########################################
######     Library compilation     #######
##########################################

# To compile a library, add a rule like this:

# $(LIBRARY_NAME):
# 	@${MAKE} $(if $(FSANITIZE),FSANITIZE=yes,) -C lib/LIBRARY_NAME

#########################################
######     Object compilation     #######
#########################################

-include $(DEPENDS)

.SECONDEXPANSION:
$(OBJS_DIR)/%.o: $(SRC_DIR)/$$(subst @,/,$$*).cpp
	@$(CC) $(CXXFLAGS) $(addprefix -iquote ,$(INCLUDE_DIR)) -c $< -o $@

$(OBJ): $(OBJS_DIR)

$(OBJS_DIR):
	@mkdir -p $(OBJS_DIR)

lib$(NAME).a: $(OBJ)
	@ar rcs $@ $(OBJ)

###############################
######      Docker      #######
###############################

test:
	@docker compose up -d --build

###############################
######     Cleaning     #######
###############################

clean: 
	@rm -f $(OBJ)

fclean: clean
	@rm -rf $(OBJS_DIR)
	@rm -f $(NAME)
	@rm -f lib$(NAME).a

re: fclean all

.PHONY: clean fclean re test