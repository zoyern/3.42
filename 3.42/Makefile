# *************************************************************** #
#                                                                   #
#                    :::::::: 3.42 ::::::::::                        #
#                                                                    #
#    By: almounib <almounib@student.42.fr>                          #
#    Project: 3.42 — Ternary Language Bootstrap                     #
#    Created: 2026/02/15                                            #
#                                                                   #
# *************************************************************** #

NAME		= s342

include build.mk

all:		$(NAME)

$(NAME):	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) -lm
	@echo "\033[32m[OK]\033[0m $(NAME) compiled"

clean:
	@rm -rf $(BUILD_DIR)
	@echo "\033[33m[CLEAN]\033[0m objects removed"

fclean:		clean
	@rm -f $(NAME)
	@echo "\033[33m[FCLEAN]\033[0m $(NAME) removed"

re:			fclean all

val:		$(NAME)
	valgrind --leak-check=full --show-leak-kinds=all ./$(NAME)

start:		all
	./$(NAME)

.PHONY:		all clean fclean re val start
