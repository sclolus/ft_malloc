/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 14:44:33 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/09 23:05:04 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include <fcntl.h>
#include <dlfcn.h>

t_malloc_info	g_malloc_info = {
	{NULL}, {{ALLOCATIONS_PER_ARENA, 0, TINY_ALLOCATION_SIZE, malloc_on_arenas, TINY_A, {0}}
			 , {ALLOCATIONS_PER_ARENA, 0, SMALL_ALLOCATION_SIZE, malloc_on_arenas, SMALL_A, {0}}
			 , {1, 0, SMALL_ALLOCATION_SIZE + 1, malloc_on_arenas, LARGE_A, {0}}}
	, DEFAULT_PAGE_SIZE, 0, {0}
};

void free(void *ptr) {
	t_arena_header *hdr;
	PRINT(1, "Attempting to free: ");
	PRINT(1, ft_static_ulltoa_base((uint64_t)ptr, HEX_BASE));
	PRINT(1, "\n");

	if (-1 == init_malloc_info() || ptr == NULL)
		return ;
	if ((hdr = find_addr_in_arenas(ptr)) == NULL)
	{
		PRINT(2, "pointer being free'd was not allocated: ");
		PRINT(2, ft_static_ulltoa_base((uint64_t)ptr, HEX_BASE));
		PRINT(2, "\n");
	}

}

static void	test_basic_arena_list_functions(void)
{
	uint32_t	i = 0;
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
		PRINT(1, "failed to init_malloc_info");
	}
	test_basic_arena_list_functions();
}

typedef void *(*malloc_func_t)(size_t);

uint32_t	*main_was_called(void)
{
	static	uint32_t	main_was_called = 0;

	return (&main_was_called);
}

void *malloc(size_t size)
{
	t_arena_type	arena_type;
	(void)size;
	(void)test_malloc;
	if (-1 == init_malloc_info())
		return (NULL);
	(void)arena_type;
	arena_type = get_arena_type_by_size(size);
	void	*ptr = malloc_on_arenas(size, g_malloc_info.arena_lists[arena_type], arena_type);
	return (ptr); // add errno

	return (NULL);
}

void *realloc(void *ptr, size_t size)
{
	void			*new_zone;
	t_arena_type	arena_type;

//	PRINT(1, "realloc() was called\n");
	if (-1 == init_malloc_info())
		return (NULL);
	arena_type = get_arena_type_by_size(size);
	new_zone = realloc_on_arenas(size, g_malloc_info.arena_lists[arena_type], arena_type, ptr);
	return (new_zone);
}

void	*calloc(size_t count, size_t size)
{
	void	*ptr;

//	PRINT(1, "calloc() was called\n");
	ptr = malloc(count * size);
	ft_bzero(ptr, count * size);
	return (ptr);
}

void	*valloc(size_t size)
{
	(void)size;
	PRINT(1, "valloc() was called ");
	if (size)
		exit(EXIT_FAILURE);
	return (NULL);
}
