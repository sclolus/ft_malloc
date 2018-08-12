ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

TEST_SRCS=srcs/test.c
TEST_NAME=test

TARGET= libft_malloc_$(HOSTTYPE).so
LN_TARGET= libft_malloc.so
NAME= $(TARGET)
SRC= srcs/malloc.c \
	srcs/arena_list.c \
	srcs/malloc_info.c \
	srcs/arena.c \
	srcs/malloc_mutex.c \
	srcs/find_addr.c \
	srcs/allocate_arena_list.c \
	srcs/deallocate_arena_list.c \
	srcs/show_alloc_mem/show_alloc_mem.c \
	srcs/show_alloc_mem/mem_print.c \
	srcs/show_alloc_mem/get_arena_number.c

HDRS= includes/ft_malloc.h
OBJ= $(SRC:.c=.o)
CC= gcc
CC_FLAGS= -v -Wall  -Wextra -Weverything  -O0 -g3 #-fsanitize=address -fsanitize-blacklist=my_ignores.txt
TARGET_CC_FLAGS= -dynamiclib -shared  -fPIC
LIBFT_PATH=./libft/
FLAGS= -I./libft/includes -I./includes

all: submodule $(NAME)

submodule:
	@make -C libft/

$(NAME): $(OBJ)
	$(CC) $(CC_FLAGS) $(FLAGS) $(TARGET_CC_FLAGS) -L./libft -lft $^  -o $(NAME)
	ln -fs  $(TARGET) $(LN_TARGET)

%.o : %.c $(HDRS)
	$(CC) $(CC_FLAGS) $(FLAGS) $< -c -o $@

clean:
	rm -f $(OBJ)
	make -C $(LIBFT_PATH) clean
fclean: clean
	rm -f $(NAME) $(TARGET) $(LN_TARGET)
	make -C $(LIBFT_PATH) fclean

re: fclean all

$(TEST_NAME): all
	$(CC) $(CC_FLAGS) $(FLAGS) $(TEST_SRCS) $(TARGET) -L./libft -lft $* -o  $(TEST_NAME)
	DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES=$(TARGET) ./$(TEST_NAME) || echo "Failure"
$(TEST_NAME)_time: all
	$(CC) $(CC_FLAGS) $(FLAGS) $(TEST_SRCS) $(TARGET) -L./libft -lft $* -o  $(TEST_NAME)
	DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES=$(TARGET) \time -l ./$(TEST_NAME) || echo "Failure"
true_malloc:
	$(CC) $(CC_FLAGS) $(FLAGS) $(TEST_SRCS) -L./libft -lft -o test_true_malloc
	\time -l ./test_true_malloc || echo "Failure"
