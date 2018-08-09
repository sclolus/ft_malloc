/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arena.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 16:17:46 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/09 23:01:59 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"


void	print_arena_type(t_arena_type type)
{
	const static char	*names[SUPPORTED_ARENA_TYPES] = {
		"tiny", "small", "large",
	};
	uint32_t	i = 0;

	while (i < sizeof(names) / sizeof(*names))
	{
		if ((int)type == (int)i)
		{
			write(1, names[i], strlen(names[i]));
		}
		i++;
	}
	PRINT(1, "\nNO TYPE FOUND\n");
}

INLINE t_arena_type	get_arena_type_by_size(size_t size)
{
	uint32_t	i;

	i = 0;
	while (i < SUPPORTED_ARENA_TYPES)
	{
		if (g_malloc_info.arena_type_infos[i].allocation_size >= size)
			return (g_malloc_info.arena_type_infos[i].type);
		i++;
	}
	return (LARGE_A); //if no type found, the size belongs to the large arena domain
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

void		*allocate_memory_on_arena(t_arena_header *hdr, uint64_t size)
{
	uint64_t	i;

	i = 0;
	while (i < g_malloc_info.arena_type_infos[hdr->arena_type].allocation_per_arena) // g_malloc_info.arena_type_infos[type].allocation_per_arena ?
	{
		if (!((hdr->arena_alloc_bitmap[i / (sizeof(t_alloc_bitmap) * 8UL)]) & (0x1UL << (63UL - i % (sizeof(t_alloc_bitmap) * 8UL)))))
			break;
		i++;
	}
	assert(i < g_malloc_info.arena_type_infos[hdr->arena_type].allocation_per_arena);
//	hdr->alloc_headers[i].size = (uint16_t)size;
//	hdr->alloc_headers[i].md5_sum = 0x0;
	(void)size;
	hdr->arena_alloc_bitmap[i / (sizeof(t_alloc_bitmap) * 8UL)] |= 0x1UL << (63UL - i % (sizeof(t_alloc_bitmap) * 8UL));
	hdr->alloc_number++;

	void	*addr = (uint8_t*)hdr->addr + i * g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size;
	assert((uint64_t)addr % 8 == 0);
	assert((uint64_t)addr % 64 == 0);
	PRINT(1, "\nallocated addr: ");
	PRINT(1, ft_static_ulltoa_base((uint64_t)addr, HEX_BASE));
	return (addr);
}

int32_t			is_addr_allocated_in_arena(void *addr, t_arena_header *hdr)
{
	uint64_t	alloc_index;
	uint64_t	allocation_size;

	if (!(hdr->addr <= addr && (uint8_t*)hdr->addr + hdr->nbr_pages * g_malloc_info.page_size > (uint8_t*)addr))
		return (0);
	allocation_size = g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size;
	alloc_index = (uint64_t)((uint8_t*)addr - (uint8_t*)hdr->addr) / allocation_size;
	if ((uint8_t*)hdr->addr + alloc_index * allocation_size != (uint8_t*)addr)
		return (0);
	return (!!(hdr->arena_alloc_bitmap[alloc_index / (sizeof(t_alloc_bitmap) * 8UL)]
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
				g_malloc_info.arena_type_infos[list->headers[i].arena_type].allocation_per_arena &&
				list->headers[i].state == USED)
				return (&list->headers[i]);
			i++;
		}
		list = list->next;
	}
	return (NULL);
}

void	*malloc_on_arenas(uint64_t size, t_arena_list *list, t_arena_type type) // remove type ?
{
	t_arena_header	*hdr;

	if (!(hdr = find_first_available_arena(list)))
		if (!(hdr = add_new_arena(list, type, size)))
			return (NULL);
	return (allocate_memory_on_arena(hdr, size));
}

void	*realloc_on_arenas(uint64_t size, t_arena_list *list, t_arena_type type, void *ptr)
{
	t_arena_header	*hdr;
	void			*new_addr;
	uint64_t		copied_size;

	if (ptr == NULL)
		return (malloc_on_arenas(size, g_malloc_info.arena_lists[type], type));
	hdr = find_addr_in_arenas(ptr);
	(void)list;
	if (hdr == NULL)
	{
		PRINT(2, "pointer being realloc'd was not allocated: ");
		PRINT(2, ft_static_ulltoa_base((uint64_t)ptr, HEX_BASE));
		PRINT(2, "\n");
		return (NULL);
	}
	if (g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size >= size)
		return (ptr);
	new_addr = malloc_on_arenas(size, g_malloc_info.arena_lists[type], type);
	copied_size = size < g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size ? size : g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size;
	if (new_addr)
		ft_memcpy(new_addr, ptr, copied_size);

	// DONT FORGET TO FREE THE OLD PTR
	return (new_addr);
}

void	free_memory_on_arena(void *addr, t_arena_header *hdr)
{
	uint64_t	allocation_size;
	uint64_t	alloc_index;

	assert(is_addr_allocated_in_arena(addr, hdr));
	hdr->alloc_number--;
	allocation_size = g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size;
	alloc_index = (uint64_t)((uint8_t*)addr - (uint8_t*)hdr->addr) / allocation_size;
	hdr->arena_alloc_bitmap[alloc_index / (sizeof(t_alloc_bitmap) * 8UL)] &= ~(0x1UL << (63UL - alloc_index % (sizeof(t_alloc_bitmap) * 8UL)));
}
