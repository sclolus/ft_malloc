/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arena.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 16:17:46 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/13 00:41:48 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

t_arena_type	get_arena_type_by_size(size_t size)
{
	uint32_t	i;

	i = 0;
	while (i < SUPPORTED_ARENA_TYPES)
	{
		if (g_malloc_info.arena_type_infos[i].allocation_size >= size)
			return (g_malloc_info.arena_type_infos[i].type);
		i++;
	}
	return (LARGE_A);
}

t_arena			*allocate_arena(uint64_t nbr_pages)
{
	t_arena	*new;

	if (MAP_FAILED ==
		(new = mmap(NULL
					, nbr_pages * g_malloc_info.page_size
					, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0)))
		return (NULL);
	return (new);
}

int32_t			deallocate_arena(t_arena *arena, uint64_t size)
{
	return (munmap(arena, size));
}

int32_t			is_addr_allocated_in_arena(void *addr, t_arena_header *hdr)
{
	uint64_t	alloc_index;
	uint64_t	allocation_size;

	if (!(hdr->addr <= addr && (uint8_t*)hdr->addr
		+ hdr->nbr_pages * g_malloc_info.page_size > (uint8_t*)addr))
		return (0);
	allocation_size = g_malloc_info.arena_type_infos[
			hdr->arena_type].allocation_size;
	alloc_index = (uint64_t)((uint8_t*)addr - (uint8_t*)hdr->addr)
											/ allocation_size;
	if ((uint8_t*)hdr->addr + alloc_index * allocation_size != (uint8_t*)addr)
		return (0);
	return (!!(hdr->arena_alloc_bitmap[alloc_index
		/ (sizeof(t_alloc_bitmap) * 8UL)]
		& (0x1UL << (63UL - alloc_index % (sizeof(t_alloc_bitmap) * 8UL)))));
}

t_arena_header	*find_first_available_arena(t_arena_list *list)
{
	uint64_t	i;

	i = 0;
	while (list)
	{
		while (i < list->capacity)
		{
			if (list->headers[i].alloc_number <
				g_malloc_info.arena_type_infos[list->headers[i].arena_type]
				.allocation_per_arena &&
				list->headers[i].state == USED)
				return (&list->headers[i]);
			i++;
		}
		list = list->next;
	}
	return (NULL);
}
