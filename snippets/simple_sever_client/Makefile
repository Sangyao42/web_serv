CC:=g++
CXXFLAGS= -std=c++98 -pedantic -Wall -Wextra -Werror -MMD -MP -O2
LDFLAGS= -std=c++98 -pedantic
ifdef FSANITIZE
	CXXFLAGS+= -g3 -fsanitize=address -DDEBUG=1
	LDFLAGS+= -g3 -fsanitize=address
endif

NAME:= client.out server.out

COMMON_SRC:= common.cpp

all: $(NAME)

$(NAME): %.out: %.cpp $(COMMON_SRC)
	$(CC) $(CXXFLAGS) -o $@ $< $(COMMON_SRC) $(LDFLAGS)

-include *.d

clean:
	$(RM) *.o *.d

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re