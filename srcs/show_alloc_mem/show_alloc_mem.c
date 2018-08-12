/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/12 22:07:30 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/13 01:42:11 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

static t_arena_header	**allocate_hdr_array(uint64_t nbr_elems)
{
	t_arena_header		**new;

	if (MAP_FAILED == (new = mmap(NULL, sizeof(t_arena_header *) * nbr_elems
				, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0)))
		return (NULL);
	return (new);
}

static t_arena_header	**fill_hdr_array(t_arena_header **hdrs,
										t_arena_list *list)
{
	uint64_t	i;
	uint64_t	elem_count;

	elem_count = 0;
	while (list)
	{
		i = 0;
		while (i < list->capacity)
		{
			if (list->headers[i].state == USED)
			{
				hdrs[elem_count] = &list->headers[i];
				elem_count++;
			}
			i++;
		}
		list = list->next;
	}
	return (hdrs);
}

static int				compar_arenas(const void *a, const void *b)
{
	void	*addr_a;
	void	*addr_b;

	addr_a = (*(const t_arena_header *const *)a)->addr;
	addr_b = (*(const t_arena_header *const *)b)->addr;
	if (addr_a > addr_b)
		return (1);
	else if (addr_a < addr_b)
		return (-1);
	else
		return (0);
}

static void				show_alloc_arenas(t_arena_list *arena_list)
{
	uint64_t		nbr_arena_hdrs;
	t_arena_header	**hdrs;

	if ((nbr_arena_hdrs = get_nbr_arena_headers(arena_list)) == 0)
		return ;
	if (NULL == (hdrs = allocate_hdr_array(nbr_arena_hdrs)))
	{
		malloc_print(ft_static_ulltoa(nbr_arena_hdrs));
		malloc_print("Failed to show_alloc_mem(): out of memory");
		return ;
	}
	fill_hdr_array(hdrs, arena_list);
	ft_sort(hdrs, nbr_arena_hdrs, sizeof(t_arena_header*), &compar_arenas);
	print_mem_arenas(hdrs, nbr_arena_hdrs);
	munmap(hdrs, sizeof(t_arena_header *) * nbr_arena_hdrs);
}

void					show_alloc_mem(void)
{
	malloc_lock_mutex();
	show_alloc_arenas(g_malloc_info.arena_lists[TINY_A]);
	show_alloc_arenas(g_malloc_info.arena_lists[SMALL_A]);
	show_alloc_arenas(g_malloc_info.arena_lists[LARGE_A]);
	malloc_unlock_mutex();
}
