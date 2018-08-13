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
	srcs/show_alloc_mem/get_arena_number.c \
	srcs/free_memory_zone.c \
	srcs/realloc_on_arenas.c \
	srcs/malloc_on_arenas.c \
	srcs/print.c

HDRS= includes/ft_malloc.h
OBJ= $(SRC:.c=.o)
CC= gcc
CC_FLAGS= -v -Wall -Werror -Wextra -Weverything -g3 #-fsanitize=address -fsanitize-blacklist=my_ignores.txt
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
