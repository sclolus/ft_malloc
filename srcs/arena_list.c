/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arena_list.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 15:16:10 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/10 04:13:53 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

t_arena_list	*init_list(t_arena_list *list, uint64_t allocated_size)
{
	ft_bzero(list, allocated_size); // remove this ?
	list->headers = (t_arena_header*)(list + 1);
	list->nbr_arenas = 0; //safe to remove for now.
	list->next = NULL;
	list->capacity = (allocated_size - sizeof(t_arena_list)) / sizeof(t_arena_header);
	PRINT(1, "\nlist->capacity: ");
	PRINT(1, ft_static_ulltoa(list->capacity));
	PRINT(1, "\n");
	return (list);
}

t_arena_list	*allocate_arena_list(void)
{
	t_arena_list	*new;
	uint64_t		allocated_size;

	assert(g_malloc_info.page_size >= sizeof(t_arena_list)); //man if that happans I dunno what to do (well I do.)
	allocated_size = g_malloc_info.page_size * ARENA_LIST_SIZE_MULTIPLE;
	if (MAP_FAILED == (new = mmap(NULL, allocated_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0)))
		return (NULL);
	init_list(new, allocated_size);
	return (new);
}

t_arena_list	*add_arena_list(t_arena_list *list)
{
	while (list->next)
		list = list->next;
	if (!((list->next = allocate_arena_list())))
		return (NULL);
	list->next->prev = list;
	return (list->next);
}

int				deallocate_arena_list(t_arena_list *list)
{
	return (munmap((void*)list, g_malloc_info.page_size * ARENA_LIST_SIZE_MULTIPLE));
}

/// the first list is never removed. ye.
t_arena_list	*remove_arena_list(t_arena_list *list)
{
	t_arena_list	*next;
	t_arena_list	*prev;

	if (list->prev == NULL)
		return (list);
	assert(list->nbr_arenas == 0);
	next = list->next;
	prev = list->prev;
	assert(deallocate_arena_list(list) == 0); // should never ever fail
	if (prev)
		prev->next = next;
	if (next)
		next->prev = prev;
	return (prev);
}

/// Find the first arena_header unused in a particular node
INLINE  t_arena_header	*find_first_unused_arena_header(t_arena_list *node)
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


t_arena_header		*find_addr_in_hdr_list(void *addr, t_arena_list *list)
{
	uint64_t	i;

	i = 0;
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
	assert(list);
	while (list)
	{
		if ((find_addr_in_hdr_list(addr, list)))
			return (list);
		list = list->next;
	}
	return (NULL);
}
///this function retrievies the arena_header from which the addr belongs to.
t_arena_list		*find_addr_in_arenas(void *addr)
{
	t_arena_list *list;

	if ((list = find_addr_in_arena_list(addr, g_malloc_info.arena_lists[TINY_A])))
		return (list);
	if ((list = find_addr_in_arena_list(addr, g_malloc_info.arena_lists[SMALL_A])))
		return (list);
	if ((list = find_addr_in_arena_list(addr, g_malloc_info.arena_lists[LARGE_A])))
		return (list);
	return (NULL);
}

INLINE t_arena_list	*retrieve_arena_list(t_arena_header *hdr)
{
//	assert(ARENA_LIST_SIZE_MULTIPLE == 1); // won't work else, I guess
	return ((t_arena_list*)((uint64_t)hdr - ((uint64_t)hdr % (ARENA_LIST_SIZE_MULTIPLE * g_malloc_info.page_size))));
}

//`size` is unused if type != LARGE_A
t_arena_header	*add_new_arena(t_arena_list *list, t_arena_type type, uint64_t size)
{
	t_arena_list	*corresponding_node;
	t_arena_header	*hdr;
	t_arena			*arena;
	uint64_t		nbr_allocated_pages;

	if (!(corresponding_node = find_first_available_arena_list(list)))
		if (!(corresponding_node = add_arena_list(list)))
			return (NULL);
	assert(hdr = find_first_unused_arena_header(corresponding_node));
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
	assert(corresponding_node->nbr_arenas <= corresponding_node->capacity);
	return (hdr);
}
