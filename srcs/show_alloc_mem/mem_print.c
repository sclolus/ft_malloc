/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mem_print.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/13 00:17:47 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/13 01:43:39 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

static const char		*get_arena_type_name(t_arena_type type)
{
	static const char	*names[SUPPORTED_ARENA_TYPES] = {
		"TINY",
		"SMALL",
		"LARGE",
	};
	uint32_t			i;

	i = 0;
	while (i < SUPPORTED_ARENA_TYPES)
	{
		if ((uint32_t)type == i)
			return (names[i]);
		i++;
	}
	return (NULL);
}

static void				print_addr(void *addr)
{
	malloc_print("0x");
	malloc_print(ft_static_ulltoa_base((uint64_t)addr
											, HEX_BASE));
}

static void				print_allocation(uint64_t index, t_arena_header *hdr)
{
	uint8_t		*allocation_addr;
	uint64_t	allocation_size;

	allocation_size = (hdr->arena_type == LARGE_A ?
					*(uint64_t*)hdr->alloc_headers :
					(uint64_t)hdr->alloc_headers[index].size);
	allocation_addr = (uint8_t*)hdr->addr + (index * allocation_size);
	print_addr(allocation_addr);
	malloc_print(" - ");
	print_addr(allocation_addr + allocation_size);
	malloc_print(" : ");
	malloc_print(ft_static_ulltoa(allocation_size));
	malloc_print(" bytes\n");
}

static void				print_arena_allocations(t_arena_header *hdr)
{
	uint64_t	i;

	malloc_print(get_arena_type_name(hdr->arena_type));
	malloc_print(" : ");
	print_addr(hdr->addr);
	malloc_print("\n");
	i = 0;
	while (i
		< g_malloc_info.arena_type_infos[hdr->arena_type].allocation_per_arena)
	{
		if (((hdr->arena_alloc_bitmap[i / (sizeof(t_alloc_bitmap) * 8UL)])
			& (0x1UL << (63UL - i % (sizeof(t_alloc_bitmap) * 8UL)))))
			print_allocation(i, hdr);
		i++;
	}
}

void					print_mem_arenas(t_arena_header **hdrs,
										uint64_t nbr_hdrs)
{
	uint64_t	i;

	i = 0;
	while (i < nbr_hdrs)
	{
		print_arena_allocations(hdrs[i]);
		i++;
	}
}
