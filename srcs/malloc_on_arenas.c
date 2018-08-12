/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_on_arenas.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/13 00:40:41 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/13 00:41:42 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

void	*malloc_on_arenas(uint64_t size,
							t_arena_list *list,
							t_arena_type type)
{
	t_arena_header	*hdr;

	if (!(hdr = find_first_available_arena(list)))
		if (!(hdr = add_new_arena(list, type, size)))
			return (NULL);
	return (allocate_memory_on_arena(hdr, size));
}

void	*allocate_memory_on_arena(t_arena_header *hdr, uint64_t size)
{
	uint64_t	i;
	void		*addr;

	i = 0;
	while (i
		< g_malloc_info.arena_type_infos[hdr->arena_type].allocation_per_arena)
	{
		if (!((hdr->arena_alloc_bitmap[i / (sizeof(t_alloc_bitmap) * 8UL)])
			& (0x1UL << (63UL - i % (sizeof(t_alloc_bitmap) * 8UL)))))
			break ;
		i++;
	}
	hdr->arena_alloc_bitmap[i / (sizeof(t_alloc_bitmap) * 8UL)] |= 0x1UL
		<< (63UL - i % (sizeof(t_alloc_bitmap) * 8UL));
	hdr->alloc_number++;
	hdr->alloc_headers[i].size = (uint16_t)size;
	if (size > SMALL_ALLOCATION_SIZE)
		*(uint64_t*)hdr->alloc_headers = size;
	addr = (uint8_t*)hdr->addr
		+ i * g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size;
	if (g_malloc_info.flags.scribble)
		ft_memset(addr, 0xaa
		, g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size);
	return (addr);
}
