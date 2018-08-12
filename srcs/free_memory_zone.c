/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_memory_zone.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/13 00:39:07 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/13 00:39:20 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

t_arena_list	*trash_arena(t_arena_header *hdr, t_arena_list *node)
{
	hdr->state = TRASHED;
	node->last_trashed_arena_header = hdr;
	ft_bzero(hdr, sizeof(*hdr));
	node->nbr_arenas--;
	if (node->nbr_arenas == 0UL)
		return (remove_arena_list(node));
	return (node);
}

void			free_memory_zone(void *addr, t_arena_list *node)
{
	uint64_t		allocation_size;
	uint64_t		alloc_index;
	t_arena_header	*hdr;

	if (addr == NULL || node == NULL)
		return ;
	hdr = find_addr_in_hdr_list(addr, node);
	if (hdr == NULL)
		return ;
	hdr->alloc_number--;
	allocation_size =
		g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size;
	alloc_index = (uint64_t)((uint8_t*)addr - (uint8_t*)hdr->addr)
											/ allocation_size;
	hdr->arena_alloc_bitmap[alloc_index / (sizeof(t_alloc_bitmap) * 8UL)] &=
		~(0x1UL << (63UL - alloc_index % (sizeof(t_alloc_bitmap) * 8UL)));
	if (g_malloc_info.flags.scribble)
		ft_memset((uint8_t*)hdr->addr + alloc_index
	* g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size
	, 0x55, g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size);
	if (hdr->alloc_number == 0)
		trash_arena(hdr, node);
}
