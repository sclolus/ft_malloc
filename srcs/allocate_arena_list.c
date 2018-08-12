/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   allocate_arena_list.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/12 20:36:29 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/12 20:36:47 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

t_arena_list	*init_list(t_arena_list *list, uint64_t allocated_size)
{
	ft_bzero(list, allocated_size);
	list->headers = (t_arena_header*)(list + 1);
	list->nbr_arenas = 0;
	list->next = NULL;
	list->capacity = (allocated_size - sizeof(t_arena_list))
										/ sizeof(t_arena_header);
	return (list);
}

t_arena_list	*allocate_arena_list(void)
{
	t_arena_list	*new;
	uint64_t		allocated_size;

	assert(g_malloc_info.page_size >= sizeof(t_arena_list));
	allocated_size = g_malloc_info.page_size * ARENA_LIST_SIZE_MULTIPLE;
	if (MAP_FAILED == (new = mmap(NULL, allocated_size, PROT_READ | PROT_WRITE
									, MAP_ANON | MAP_PRIVATE, -1, 0)))
		return (NULL);
	init_list(new, allocated_size);
	return (new);
}
