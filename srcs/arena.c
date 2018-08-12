/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arena.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 16:17:46 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/13 00:09:11 by sclolus          ###   ########.fr       */
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

int32_t		deallocate_arena(t_arena *arena, uint64_t size)
{
	return (munmap(arena, size));
}

void		*allocate_memory_on_arena(t_arena_header *hdr, uint64_t size)
{
	uint64_t	i;

	i = 0;
	while (i < g_malloc_info.arena_type_infos[hdr->arena_type].allocation_per_arena)
	{
		if (!((hdr->arena_alloc_bitmap[i / (sizeof(t_alloc_bitmap) * 8UL)]) & (0x1UL << (63UL - i % (sizeof(t_alloc_bitmap) * 8UL)))))
			break;
		i++;
	}
	assert(i < g_malloc_info.arena_type_infos[hdr->arena_type].allocation_per_arena);
	hdr->arena_alloc_bitmap[i / (sizeof(t_alloc_bitmap) * 8UL)] |= 0x1UL << (63UL - i % (sizeof(t_alloc_bitmap) * 8UL));
	hdr->alloc_number++;
	hdr->alloc_headers[i].size = (uint16_t)size;
	if (size > SMALL_ALLOCATION_SIZE)
		*(uint64_t*)hdr->alloc_headers = size;
	void	*addr = (uint8_t*)hdr->addr + i * g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size;
	assert((uint64_t)addr % 8 == 0);
	assert((uint64_t)addr % 64 == 0);
	if (g_malloc_info.flags.scribble)
		ft_memset(addr, 0xaa, g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size);
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

	void	*ptr =  allocate_memory_on_arena(hdr, size);
	assert((uint64_t)ptr % 16 == 0);
	return (ptr);
}

void	*realloc_on_arenas(uint64_t size, t_arena_list *list, t_arena_type type, void *ptr)
{
	t_arena_header	*hdr;
	void			*new_addr;
	uint64_t		copied_size;
	uint64_t		old_size;

	if (ptr == NULL)
		return (malloc_on_arenas(size, g_malloc_info.arena_lists[type], type));
	list = find_addr_in_arenas(ptr);
	hdr = find_addr_in_hdr_list(ptr, list);
	if (hdr == NULL || list == NULL)
	{
		PRINT(g_malloc_info.fd_output, "pointer being realloc'd was not allocated: ");
		PRINT(g_malloc_info.fd_output, ft_static_ulltoa_base((uint64_t)ptr, HEX_BASE));
		PRINT(g_malloc_info.fd_output, "\n");
		if (g_malloc_info.flags.error_abort)
			abort();
		return (NULL);
	}
	if (g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size >= size)
		return (ptr);
	new_addr = malloc_on_arenas(size, g_malloc_info.arena_lists[type], type);
	old_size = hdr->arena_type != LARGE_A ? g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size
		: hdr->nbr_pages * g_malloc_info.page_size;
	size = type != LARGE_A ? g_malloc_info.arena_type_infos[type].allocation_size : size;
	copied_size = old_size > size ? size : old_size;
	if (!size)
		copied_size = TINY_ALLOCATION_SIZE;
	if (new_addr)
		ft_memcpy(new_addr, ptr, copied_size);
	free_memory_zone(ptr, list);
	return (new_addr);
}

t_arena_list *trash_arena(t_arena_header *hdr, t_arena_list *node)
{
	assert(hdr && node);

	hdr->state = TRASHED;
	node->last_trashed_arena_header = hdr;
	assert(deallocate_arena(hdr->addr, hdr->nbr_pages * g_malloc_info.page_size) == 0);
	ft_bzero(hdr, sizeof(*hdr)); // not sure I should leave it
	node->nbr_arenas--;
	if (node->nbr_arenas == 0UL)
		return (remove_arena_list(node));
	return (node);
}

void	free_memory_zone(void *addr, t_arena_list *node)
{
	uint64_t		allocation_size;
	uint64_t		alloc_index;
	t_arena_header	*hdr;

	if (addr == NULL || node == NULL)
		return ;
	hdr = find_addr_in_hdr_list(addr, node);
	assert(hdr);
	if (hdr == NULL)
		return ;
	hdr->alloc_number--;
	allocation_size = g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size;
	alloc_index = (uint64_t)((uint8_t*)addr - (uint8_t*)hdr->addr) / allocation_size;
	hdr->arena_alloc_bitmap[alloc_index / (sizeof(t_alloc_bitmap) * 8UL)] &= ~(0x1UL << (63UL - alloc_index % (sizeof(t_alloc_bitmap) * 8UL)));
	if (g_malloc_info.flags.scribble)
		ft_memset((uint8_t*)hdr->addr + alloc_index * g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size
				  , 0x55, g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size);
	if (hdr->alloc_number == 0)
		trash_arena(hdr, node);
}
