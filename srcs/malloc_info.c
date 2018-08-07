/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_info.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 15:13:35 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/07 16:20:43 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

INLINE void	init_malloc_info(void)
{
	if (g_malloc_info.initialized)
		return ;
	g_malloc_info.page_size = (uint64_t)getpagesize();
	g_malloc_info.initialized = 1;
	if (!(g_malloc_info.arena_lists[0] = allocate_arena_list()) ||
		!(g_malloc_info.arena_lists[1] = allocate_arena_list()) ||
		!(g_malloc_info.arena_lists[2] = allocate_arena_list()))
		abort(); // remove this
	g_malloc_info.arena_size[TINY_A] = TINY_ARENA_SIZE / g_malloc_info.page_size;
	g_malloc_info.arena_size[SMALL_A] = SMALL_ARENA_SIZE / g_malloc_info.page_size;
	g_malloc_info.arena_size[LARGE_A] = (SMALL_ARENA_SIZE + 1) / g_malloc_info.page_size; //this does not matter
}
