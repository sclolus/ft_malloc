/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_info.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 15:13:35 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/10 08:48:30 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

pthread_mutex_t	g_malloc_mutex;

INLINE void		malloc_lock_mutex(void)
{
	assert(pthread_mutex_lock(&g_malloc_mutex) == 0);
}

INLINE void		malloc_unlock_mutex(void)
{
	assert(pthread_mutex_unlock(&g_malloc_mutex) == 0);
}

INLINE int32_t	init_malloc_info(void)
{
	pthread_mutexattr_t	attr;
	char				*filename;

	if (g_malloc_info.initialized)
		return (0);
	pthread_mutexattr_init(&attr); // check those ?
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&g_malloc_mutex, &attr);
	g_malloc_info.page_size = (uint64_t)getpagesize();
	g_malloc_info.initialized = 1;
	if (!(g_malloc_info.arena_lists[0] = allocate_arena_list()) ||
		!(g_malloc_info.arena_lists[1] = allocate_arena_list()) ||
		!(g_malloc_info.arena_lists[2] = allocate_arena_list()))
		return (-1);
	g_malloc_info.arena_type_infos[TINY_A].nbr_pages = TINY_ARENA_SIZE / g_malloc_info.page_size;
	g_malloc_info.arena_type_infos[SMALL_A].nbr_pages = SMALL_ARENA_SIZE / g_malloc_info.page_size;
	g_malloc_info.arena_type_infos[LARGE_A].nbr_pages = (SMALL_ARENA_SIZE + 1) / g_malloc_info.page_size; //this does not matter
	if ((filename = getenv("MallocLogFil")) == NULL)
		return (0);
	if (-1 == (g_malloc_info.fd_output = open(filename, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR)))
		return (-1);
	return (0);
}
