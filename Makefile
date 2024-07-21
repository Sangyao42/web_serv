################################
######     Variables     #######
################################

CC:=c++
CXXFLAGS= -std=c++98 -pedantic -Wall -Wextra -Werror -MMD -MP
LDFLAGS= -std=c++98 -pedantic
ifdef DEBUG
	CXXFLAGS+= -g3 -DDEBUG=1
	LDFLAGS+= -g3 
endif

ifdef FSANITIZE
	CXXFLAGS+= -g3 -fsanitize=address -DDEBUG=1
	LDFLAGS+= -g3 -fsanitize=address
endif

ifndef FSANITIZE
ifndef DEBUG
	CXXFLAGS+= -O2 -DNDEBUG
endif
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
	constants.cpp \
	Cgi.cpp \
	ClientLifespan.cpp \
	FileManipulation.cpp \
	HTTPMessage.cpp \
	Process.cpp \
	Request.cpp \
	Response.cpp

ARENA_SRC:= \
	Arenas.cpp \
	Arena/Arena.cpp

URI_SRC:= \
	Uri/Authority.cpp

HTTP_SRC:= \
	Http/Parser.cpp

CONFIGURATION_SRC:= \
	Configuration.cpp \
	Configuration/Parser.cpp \
	Configuration/Directive.cpp \
	Configuration/Cache/LocationQuery.cpp \
	Configuration/Cache/ServerQuery.cpp \
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

SOCKETMANAGER_SRC:= \
	socket_manager/SocketManager.cpp

HEADERVALUE_SRC:= \
	HeaderValue/HeaderInt.cpp \
	HeaderValue/HeaderString.cpp \
	HeaderValue/HeaderStringVector.cpp

RESBUILDER_SRC:= \
	ResBuilder/ResBuilderAutoindex.cpp \
	ResBuilder/ResBuilderError.cpp \
	ResBuilder/ResBuilderRedirect.cpp \
	ResBuilder/ResBuilderSuccess.cpp \
	ResBuilder/ResBuilderUtils.cpp

SRC:= $(MAIN_SRC) $(ARENA_SRC) $(URI_SRC) $(HTTP_SRC) $(CONFIGURATION_SRC) $(MISC_SRC) $(SOCKETMANAGER_SRC) $(HEADERVALUE_SRC) $(RESBUILDER_SRC)

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
$(OBJS_DIR)/%.o:  $(SRC_DIR)/$$(subst @,/,$$*).cpp | $(OBJS_DIR)
	$(CC) $(CXXFLAGS) $(addprefix -iquote ,$(INCLUDE_DIR)) -c $< -o $@

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
