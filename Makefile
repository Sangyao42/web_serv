################################
######     Variables     #######
################################

CC:=g++
CXXFLAGS= -std=c++98 -pedantic -Wall -Wextra -Werror -MMD -MP -O2
LDFLAGS= -std=c++98 -pedantic
ifdef FSANITIZE
	CXXFLAGS+= -g3 -fsanitize=address -DDEBUG=1
	LDFLAGS+= -g3 -fsanitize=address
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
	main.cpp

CONFIGURATION_SRC:= \
	Configuration/Directive.cpp \
	Configuration/Directive/Http.cpp \
	Configuration/Directive/Server.cpp \
	Configuration/Directive/Events.cpp \
	Configuration/Directive/Location.cpp \
	Configuration/Directive/Listen.cpp \
	Configuration/Directive/ServerName.cpp \
	Configuration/Directive/AllowMethods.cpp \
	Configuration/Directive/MimeTypes.cpp \
	Configuration/Directive/ErrorPage.cpp \
	Configuration/Directive/Redirect.cpp \
	Configuration/Directive/Cgi.cpp

MISC_SRC:= \
	misc/Nothing.cpp

SRC:= $(MAIN_SRC) $(CONFIGURATION_SRC) $(MISC_SRC)

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

docker_start:
	@docker compose up -d --build

docker_test:
	@docker exec -it web_serv make -C spec run

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

.PHONY: clean fclean re docker_start docker_test