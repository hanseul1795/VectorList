NAME    = vectorProject
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
CC  = g++
CXXFLAGS= -std=c++11 -W -Wall -Wextra
RM  = rm -f
ECHO    = echo -e
all : $(NAME)
$(NAME) : $(OBJ)
	@$(CC) -o $(NAME) $(OBJ)
	@$(ECHO) '\033[0;32m> Compiled\033[0m'

clean   :

	@$(RM) $(OBJ)
	@$(RM) *~
	@$(RM) \#*#
	@$(ECHO) '\033[0;31m> Directory cleaned\033[0m'

fclean  : clean

	@$(RM) $(NAME)
	@$(ECHO) '\033[0;31m> Remove executable\033[0m'

re  : fclean all
