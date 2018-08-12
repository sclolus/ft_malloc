/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   deallocate_arena_list.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/12 20:37:15 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/12 20:37:20 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

int				deallocate_arena_list(t_arena_list *list)
{
	return (munmap((void*)list
		, g_malloc_info.page_size * ARENA_LIST_SIZE_MULTIPLE));
}
