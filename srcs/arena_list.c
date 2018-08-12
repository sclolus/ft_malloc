/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arena_list.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 15:16:10 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/12 20:39:18 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

t_arena_list		*add_arena_list(t_arena_list *list)
{
	while (list->next)
		list = list->next;
	if (!((list->next = allocate_arena_list())))
		return (NULL);
	list->next->prev = list;
	return (list->next);
}

t_arena_list		*remove_arena_list(t_arena_list *list)
{
	t_arena_list	*next;
	t_arena_list	*prev;

	if (list->prev == NULL)
		return (list);
	assert(list->nbr_arenas == 0);
	next = list->next;
	prev = list->prev;
	assert(deallocate_arena_list(list) == 0);
	if (prev)
		prev->next = next;
	if (next)
		next->prev = prev;
	return (prev);
}

t_arena_header		*find_first_unused_arena_header(t_arena_list *node)
{
	uint64_t	i;

	assert(node);
	i = 0;
	if (node->last_trashed_arena_header)
		return (node->last_trashed_arena_header);
	while (i < node->capacity)
	{
		if (node->headers[i].state == TRASHED)
			return (&node->headers[i]);
		i++;
	}
	return (NULL);
}

t_arena_list		*find_first_available_arena_list(t_arena_list *list)
{
	assert(list);
	while (list)
	{
		if (list->nbr_arenas < list->capacity)
			return (list);
		list = list->next;
	}
	return (NULL);
}

t_arena_header		*add_new_arena(t_arena_list *list,
								t_arena_type type,
								uint64_t size)
{
	t_arena_list	*corresponding_node;
	t_arena_header	*hdr;
	t_arena			*arena;
	uint64_t		nbr_allocated_pages;

	if (!(corresponding_node = find_first_available_arena_list(list)))
		if (!(corresponding_node = add_arena_list(list)))
			return (NULL);
	hdr = find_first_unused_arena_header(corresponding_node);
	corresponding_node->last_trashed_arena_header = NULL;
	nbr_allocated_pages = (type != LARGE_A)
		? g_malloc_info.arena_type_infos[type].nbr_pages
		: size / g_malloc_info.page_size + !!(size % g_malloc_info.page_size);
	if (!(arena = allocate_arena(nbr_allocated_pages)))
		return (NULL);
	corresponding_node->nbr_arenas++;
	ft_bzero(hdr, sizeof(t_arena_header));
	hdr->addr = arena;
	hdr->state = USED;
	hdr->nbr_pages = nbr_allocated_pages;
	hdr->alloc_number = 0UL;
	hdr->arena_alloc_bitmap[0] = 0UL;
	hdr->arena_alloc_bitmap[1] = 0UL;
	hdr->arena_type = type;
	return (hdr);
}
