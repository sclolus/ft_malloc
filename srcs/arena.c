/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arena.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 16:17:46 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/07 18:16:29 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

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

t_arena			*allocate_arena(enum e_arena_type type)
{
	t_arena	*new;

	if (MAP_FAILED ==
		(new = mmap(NULL
					, g_malloc_info.arena_type_infos[type].nbr_pages
					, PROT_READ | PROT_WRITE, MAP_ANON, -1, 0)))
		return (NULL);
	return (new);
}
