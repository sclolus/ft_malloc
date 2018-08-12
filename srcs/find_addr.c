/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_addr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/12 20:34:43 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/13 01:18:10 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

t_arena_header		*find_addr_in_hdr_list(void *addr, t_arena_list *list)
{
	uint64_t	i;

	i = 0;
	if (list == NULL)
		return (NULL);
	while (i < list->capacity)
	{
		if (list->headers[i].state == USED &&
			is_addr_allocated_in_arena(addr, &list->headers[i]))
			return (&list->headers[i]);
		i++;
	}
	return (NULL);
}

t_arena_list		*find_addr_in_arena_list(void *addr, t_arena_list *list)
{
	while (list)
	{
		if ((find_addr_in_hdr_list(addr, list)))
			return (list);
		list = list->next;
	}
	return (NULL);
}

t_arena_list		*find_addr_in_arenas(void *addr)
{
	t_arena_list *list;

	if ((list = find_addr_in_arena_list(addr
										, g_malloc_info.arena_lists[TINY_A])))
		return (list);
	if ((list = find_addr_in_arena_list(addr
										, g_malloc_info.arena_lists[SMALL_A])))
		return (list);
	if ((list = find_addr_in_arena_list(addr
										, g_malloc_info.arena_lists[LARGE_A])))
		return (list);
	return (NULL);
}
