/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_info.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 15:13:35 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/13 02:12:34 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

CONSTRUCTOR	main_was_called(void)
{
	g_malloc_info.main_was_called = 1;
}

static void	open_log_file(void)
{
	char				*filename;

	if ((filename = getenv("MallocLogFil")))
	{
		if (-1 == (g_malloc_info.fd_output = open(filename, O_CREAT | O_TRUNC
												| O_WRONLY, S_IRUSR | S_IWUSR)))
		{
			g_malloc_info.fd_output = STDERR_FILENO;
			if (!g_malloc_info.flags.quiet)
			{
				malloc_print("Failed to open log file: ");
				malloc_print(filename);
				malloc_print("\n");
			}
		}
		g_malloc_info.flags.log_file = 1;
	}
	else
		g_malloc_info.fd_output = STDERR_FILENO;
}

static void	set_malloc_flags(void)
{
	if (getenv("MallocQuiet"))
		g_malloc_info.flags.quiet = 1;
	open_log_file();
	if (getenv("MallocScribble"))
	{
		if (!g_malloc_info.flags.quiet)
			malloc_print(
				"malloc: enabling scribbling to detect mods to free blocks\n");
		g_malloc_info.flags.scribble = 1;
	}
	if (getenv("MallocErrorAbort"))
	{
		if (!g_malloc_info.flags.quiet)
			malloc_print(
				"malloc: enabling abort() on bad malloc or free\n");
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
