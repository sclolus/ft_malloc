/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 14:44:33 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/12 17:19:51 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include <fcntl.h>
#include <dlfcn.h>

t_malloc_info	g_malloc_info = {
	{NULL}, {{ALLOCATIONS_PER_ARENA, 0, TINY_ALLOCATION_SIZE, malloc_on_arenas, TINY_A, {0}}
			 , {ALLOCATIONS_PER_ARENA, 0, SMALL_ALLOCATION_SIZE, malloc_on_arenas, SMALL_A, {0}}
			 , {1, 0, SMALL_ALLOCATION_SIZE + 1, malloc_on_arenas, LARGE_A, {0}}}
	, DEFAULT_PAGE_SIZE, STDOUT_FILENO, 0, {0}
};

void free(void *ptr) {
	t_arena_list *node;

	PRINT(g_malloc_info.fd_output, "\nAttempting to free: ");
	PRINT(g_malloc_info.fd_output, ft_static_ulltoa_base((uint64_t)ptr, HEX_BASE));
	PRINT(g_malloc_info.fd_output, "\n");

	if (-1 == init_malloc_info() || ptr == NULL)
	{
		PRINT(g_malloc_info.fd_output, "\free was exited: ");
		return ;
	}
	malloc_lock_mutex();
	if ((node = find_addr_in_arenas(ptr)) == NULL)
	{
		if (g_main_was_called) {
			PRINT(2, "pointer being free'd was not allocated: ");
			PRINT(2, ft_static_ulltoa_base((uint64_t)ptr, HEX_BASE));
			PRINT(2, "\n");
		}
		malloc_unlock_mutex();
		PRINT(g_malloc_info.fd_output, "\free was exited: ");
		return ;
	}
	free_memory_zone(ptr, node);
	malloc_unlock_mutex();
	PRINT(g_malloc_info.fd_output, "\free was exited: ");
}

static void	test_basic_arena_list_functions(void)
{
	uint32_t		i = 0;

	while (i < 128) {
		assert(add_arena_list(g_malloc_info.arena_lists[1]));
		i++;
	}
	i = 0;
	while (i < 128)
	{
		assert(remove_arena_list(g_malloc_info.arena_lists[1]->next));
		i++;
	}
	assert(g_malloc_info.arena_lists[1]->next == NULL);
}

static void	test_malloc(void)
{
	if (g_malloc_info.initialized)
	{
		PRINT(2, "Did not test malloc since it was already used before\n");
		return ;
	}
	if (-1 == init_malloc_info())
	{
		PRINT(g_malloc_info.fd_output, "failed to init_malloc_info");
	}
	test_basic_arena_list_functions();
}

typedef void *(*malloc_func_t)(size_t);

uint32_t	g_main_was_called = 0;

__attribute__((constructor(99999))) void main_was_called(void)
{
	g_main_was_called = 1;
}

void *malloc(size_t size)
{
	t_arena_type	arena_type;
	void			*ptr;
	(void)size;
	(void)test_malloc;
	PRINT(g_malloc_info.fd_output, "malloc() was called\n");
	if (-1 == init_malloc_info())
		return (NULL);
	(void)arena_type;
	malloc_lock_mutex();
	arena_type = get_arena_type_by_size(size);
	ptr = malloc_on_arenas(size, g_malloc_info.arena_lists[arena_type], arena_type);
	malloc_unlock_mutex();
	PRINT(g_malloc_info.fd_output, "size : ");
	PRINT(g_malloc_info.fd_output, ft_static_ulltoa(size));
	PRINT(g_malloc_info.fd_output, "malloc() was exited\n");
	return (ptr); // add errno
}

void *realloc(void *ptr, size_t size)
{
	void			*new_zone;
	t_arena_type	arena_type;

	PRINT(g_malloc_info.fd_output, "realloc() was called\n");
	if (-1 == init_malloc_info())
		return (NULL);
	malloc_lock_mutex();
	arena_type = get_arena_type_by_size(size);
	new_zone = realloc_on_arenas(size, g_malloc_info.arena_lists[arena_type], arena_type, ptr);
	malloc_unlock_mutex();
	return (new_zone);
}


void *reallocf(void *ptr, size_t size) // not completed
{
	void			*new_zone;
	t_arena_type	arena_type;

	PRINT(g_malloc_info.fd_output, "reallocf() was called\n");
	if (-1 == init_malloc_info())
		return (NULL);
	malloc_lock_mutex();
	arena_type = get_arena_type_by_size(size);
	if (!(new_zone = realloc_on_arenas(size, g_malloc_info.arena_lists[arena_type], arena_type, ptr)))
		free(ptr);
	malloc_unlock_mutex();
	return (new_zone);
}

void	*calloc(size_t count, size_t size)
{
	void	*ptr;

	if (-1 == init_malloc_info())
		return (NULL);
	PRINT(g_malloc_info.fd_output, "calloc() was called\n");
	malloc_lock_mutex();
	ptr = malloc(count * size);
	ft_bzero(ptr, count * size);
	malloc_unlock_mutex();
	return (ptr);
}

void	*valloc(size_t size)
{
	(void)size;
	PRINT(g_malloc_info.fd_output, "valloc() was called ");
	assert(0);
	if (size)
		exit(EXIT_FAILURE);
	return (NULL);
}
