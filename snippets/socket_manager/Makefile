CC:=g++
CXXFLAGS= -std=c++98 -pedantic -Wall -Wextra -Werror -MMD -MP -O2
LDFLAGS= -std=c++98 -pedantic
ifdef FSANITIZE
	CXXFLAGS+= -g3 -fsanitize=address -DDEBUG=1
	LDFLAGS+= -g3 -fsanitize=address
endif

NAME:= addrinfo.out
SRC:= addrinfo.cpp
SRCS:= $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(SRCS)
	$(CC) $(CXXFLAGS) -o $@ $(SRCS) $(LDFLAGS)

-include *.d

clean:
	$(RM) *.o *.d

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
