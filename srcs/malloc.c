/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 14:44:33 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/12 20:29:05 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include <fcntl.h>
#include <dlfcn.h>

t_malloc_info	g_malloc_info = {
	{NULL}, {{ALLOCATIONS_PER_ARENA, 0, TINY_ALLOCATION_SIZE, TINY_A, {0}}, {
			ALLOCATIONS_PER_ARENA, 0, SMALL_ALLOCATION_SIZE, SMALL_A, {0}}, {
			1, 0, SMALL_ALLOCATION_SIZE + 1, LARGE_A, {0}}}
	, DEFAULT_PAGE_SIZE, STDOUT_FILENO, {0, 0, 0, 0}, 0, 0
};

void	free(void *ptr)
{
	t_arena_list *node;

	if (-1 == init_malloc_info() || ptr == NULL)
		return ;
	malloc_lock_mutex();
	if ((node = find_addr_in_arenas(ptr)) == NULL)
	{
		if (g_malloc_info.main_was_called)
		{
			PRINT(g_malloc_info.fd_output
				, "pointer being free'd was not allocated: ");
			PRINT(g_malloc_info.fd_output
				, ft_static_ulltoa_base((uint64_t)ptr, HEX_BASE));
			PRINT(g_malloc_info.fd_output, "\n");
			if (g_malloc_info.flags.error_abort)
				abort();
		}
		malloc_unlock_mutex();
		return ;
	}
	free_memory_zone(ptr, node);
	malloc_unlock_mutex();
}

void	*malloc(size_t size)
{
	t_arena_type	arena_type;
	void			*ptr;

	if (-1 == init_malloc_info())
		return (NULL);
	malloc_lock_mutex();
	arena_type = get_arena_type_by_size(size);
	ptr = malloc_on_arenas(size, g_malloc_info.arena_lists[arena_type]
						, arena_type);
	malloc_unlock_mutex();
	return (ptr);
}

void	*realloc(void *ptr, size_t size)
{
	void			*new_zone;
	t_arena_type	arena_type;

	if (-1 == init_malloc_info())
		return (NULL);
	malloc_lock_mutex();
	arena_type = get_arena_type_by_size(size);
	new_zone = realloc_on_arenas(size, g_malloc_info.arena_lists[arena_type]
								, arena_type, ptr);
	malloc_unlock_mutex();
	return (new_zone);
}

void	*reallocf(void *ptr, size_t size)
{
	void			*new_zone;
	t_arena_type	arena_type;

	if (-1 == init_malloc_info())
		return (NULL);
	malloc_lock_mutex();
	arena_type = get_arena_type_by_size(size);
	if (!(new_zone = realloc_on_arenas(size
					, g_malloc_info.arena_lists[arena_type], arena_type, ptr)))
		free(ptr);
	malloc_unlock_mutex();
	return (new_zone);
}

void	*calloc(size_t count, size_t size)
{
	void	*ptr;

	if (-1 == init_malloc_info())
		return (NULL);
	malloc_lock_mutex();
	ptr = malloc(count * size);
	ft_bzero(ptr, count * size);
	malloc_unlock_mutex();
	return (ptr);
}
