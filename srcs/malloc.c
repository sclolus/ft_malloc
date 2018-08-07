/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 14:44:33 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/07 16:17:22 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

t_malloc_info	g_malloc_info = {
	{NULL}, {0}, DEFAULT_PAGE_SIZE, 0, {0}
};

void free(void *ptr) {
	(void)ptr;
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
		dprintf(2, "Did not test malloc since it was already used before\n");
		return ;
	}
	test_basic_arena_list_functions();
}

void *malloc(size_t size)
{
	(void)size;
	init_malloc_info();
	test_malloc();

	return (NULL);
}

void *realloc(void *ptr, size_t size)
{
	(void)ptr;
	(void)size;
	return (NULL);
}
