/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_info.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 15:13:35 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/12 20:21:15 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

pthread_mutex_t	g_malloc_mutex;

void		malloc_lock_mutex(void)
{
	assert(pthread_mutex_lock(&g_malloc_mutex) == 0);
}

void		malloc_unlock_mutex(void)
{
	assert(pthread_mutex_unlock(&g_malloc_mutex) == 0);
}

static void	open_log_file(void)
{
	char				*filename;

	if ((filename = getenv("MallocLogFil")))
	{
		if (-1 == (g_malloc_info.fd_output = open(filename, O_CREAT | O_TRUNC
												| O_WRONLY, S_IRUSR | S_IWUSR)))
		{
			PRINT(STDERR_FILENO, "Failed to open log file: ");
			PRINT(STDERR_FILENO, filename);
			PRINT(STDERR_FILENO, "\n");
			g_malloc_info.fd_output = STDERR_FILENO;
		}
		g_malloc_info.flags.log_file = 1;
	}
	else
		g_malloc_info.fd_output = STDERR_FILENO;
}

static void	set_malloc_flags(void)
{
	open_log_file();
	if (getenv("MallocScribble"))
	{
		PRINT(g_malloc_info.fd_output
			, "malloc: enabling scribbling to detect mods to free blocks\n");
		g_malloc_info.flags.scribble = 1;
	}
	if (getenv("MallocErrorAbort"))
	{
		PRINT(g_malloc_info.fd_output
			, "malloc: enabling abort() on bad malloc or free\n");
		g_malloc_info.flags.error_abort = 1;
	}
}

int32_t		init_malloc_info(void)
{
	pthread_mutexattr_t	attr;

	if (g_malloc_info.initialized)
		return (0);
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&g_malloc_mutex, &attr);
	g_malloc_info.page_size = (uint64_t)getpagesize();
	g_malloc_info.initialized = 1;
	if (!(g_malloc_info.arena_lists[0] = allocate_arena_list()) ||
		!(g_malloc_info.arena_lists[1] = allocate_arena_list()) ||
		!(g_malloc_info.arena_lists[2] = allocate_arena_list()))
		return (-1);
	g_malloc_info.arena_type_infos[TINY_A].nbr_pages = TINY_ARENA_SIZE /
		g_malloc_info.page_size;
	g_malloc_info.arena_type_infos[SMALL_A].nbr_pages = SMALL_ARENA_SIZE /
		g_malloc_info.page_size;
	g_malloc_info.arena_type_infos[LARGE_A].nbr_pages = (SMALL_ARENA_SIZE + 1) /
		g_malloc_info.page_size;
	set_malloc_flags();
	return (0);
}
