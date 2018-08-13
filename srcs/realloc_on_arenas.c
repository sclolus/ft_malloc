/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc_on_arenas.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/13 00:39:51 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/13 02:11:09 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

static void	*realloc_error_not_allocated(void *ptr)
{
	if (!g_malloc_info.flags.quiet)
	{
		malloc_print("pointer being realloc'd was not allocated: ");
		malloc_print(ft_static_ulltoa_base((uint64_t)ptr, HEX_BASE));
		malloc_print("\n");
	}
	if (g_malloc_info.flags.error_abort)
		abort();
	return (NULL);
}

void		*realloc_on_arenas(uint64_t size,
							t_arena_list *list,
							t_arena_type type,
							void *ptr)
{
	t_arena_header	*hdr;
	void			*new_addr;
	uint64_t		copied_size;
	uint64_t		old_size;

	if (ptr == NULL)
		return (malloc_on_arenas(size, g_malloc_info.arena_lists[type], type));
	list = find_addr_in_arenas(ptr);
	if ((hdr = find_addr_in_hdr_list(ptr, list)) == NULL || list == NULL)
		return (realloc_error_not_allocated(ptr));
	if (g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size >= size)
		return (ptr);
	new_addr = malloc_on_arenas(size, g_malloc_info.arena_lists[type], type);
	old_size = hdr->arena_type != LARGE_A
		? g_malloc_info.arena_type_infos[hdr->arena_type].allocation_size
		: hdr->nbr_pages * g_malloc_info.page_size;
	size = type != LARGE_A ?
		g_malloc_info.arena_type_infos[type].allocation_size : size;
	copied_size = old_size > size ? size : old_size;
	if (!size)
		copied_size = TINY_ALLOCATION_SIZE;
	if (new_addr)
		ft_memcpy(new_addr, ptr, copied_size);
	free_memory_zone(ptr, list);
	return (new_addr);
}
