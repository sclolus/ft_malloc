ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

TEST_SRCS=srcs/test.c
TEST_NAME=test

TARGET= libft_malloc_$(HOSTTYPE).so
NAME= $(TARGET)
SRC= srcs/malloc.c \
	srcs/arena_list.c \
	srcs/malloc_info.c \
	srcs/arena.c
HDRS= includes/ft_malloc.h
OBJ= $(SRC:.c=.o)
CC= gcc
CC_FLAGS= -v -Wall -Werror -Wextra -Weverything  -O0 -g3 #-fsanitize=address -fsanitize-blacklist=my_ignores.txt
TARGET_CC_FLAGS= -dynamic -shared  -fpic
LIBFT_PATH=./libft/
FLAGS= -I./libft/includes -I./includes

all: submodule $(NAME)

submodule:
	@make -C libft/

$(NAME): $(OBJ)
	$(CC) $(CC_FLAGS) $(FLAGS) $(TARGET_CC_FLAGS) -L./libft -lft $^  -o $(NAME)
%.o : %.c $(HDRS)
	$(CC) $(CC_FLAGS) $(FLAGS) $< -c -o $@

clean:
	rm -f $(OBJ)
	make -C $(LIBFT_PATH) clean
fclean: clean
	rm -f $(NAME)
	make -C $(LIBFT_PATH) fclean

re: fclean all

$(TEST_NAME): all
	$(CC) $(CC_FLAGS) $(FLAGS) $(TEST_SRCS) $(TARGET) -L./libft -lft $* -o  $(TEST_NAME)
	DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES=$(TARGET) ./$(TEST_NAME) || echo "Failure"
$(TEST_NAME)_time: all
	$(CC) $(CC_FLAGS) $(FLAGS) $(TEST_SRCS) $(TARGET) -L./libft -lft $* -o  $(TEST_NAME)
	DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES=$(TARGET) \time -l ./$(TEST_NAME) || echo "Failure"
true_malloc: $(TEST_NAME)
	$(CC) $(CC_FLAGS) $(FLAGS) $(TEST_SRCS) -L./libft -lft -o test_true_malloc
	\time -l ./test_true_malloc || echo "Failure"
