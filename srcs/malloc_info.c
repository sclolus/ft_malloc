/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_info.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 15:13:35 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/10 06:11:49 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

pthread_mutexattr_t	g_malloc_mutex;

INLINE void		cleanup_unlock_mutex(void **__attribute__((unused))no)
{
	assert(pthread_mutex_unlock(&g_malloc_mutex) == 0);
}

INLINE int32_t	init_malloc_info(void)
{
	if (g_malloc_info.initialized)
	{
		assert(pthread_mutex_lock(&g_malloc_mutex) == 0);
		return (0);
	}
	pthread_mutexattr_init(&g_malloc_mutex);
	pthread_mutexattr_settype(&g_malloc_mutex, PTHREAD_MUTEX_RECURSIVE);
	g_malloc_info.page_size = (uint64_t)getpagesize();
	g_malloc_info.initialized = 1;
	if (!(g_malloc_info.arena_lists[0] = allocate_arena_list()) ||
		!(g_malloc_info.arena_lists[1] = allocate_arena_list()) ||
		!(g_malloc_info.arena_lists[2] = allocate_arena_list()))
		return (-1);
	g_malloc_info.arena_type_infos[TINY_A].nbr_pages = TINY_ARENA_SIZE / g_malloc_info.page_size;
	g_malloc_info.arena_type_infos[SMALL_A].nbr_pages = SMALL_ARENA_SIZE / g_malloc_info.page_size;
	g_malloc_info.arena_type_infos[LARGE_A].nbr_pages = (SMALL_ARENA_SIZE + 1) / g_malloc_info.page_size; //this does not matter
	return (0);
}
