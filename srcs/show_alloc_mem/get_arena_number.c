/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_arena_number.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/13 00:20:47 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/13 00:22:25 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

static uint64_t	get_nbr_arena_hdrs_in_node(t_arena_list *list)
{
	uint64_t	i;
	uint64_t	nbr_hdrs;

	i = 0;
	nbr_hdrs = 0;
	while (i < list->capacity)
	{
		if (list->headers[i].state == USED)
			nbr_hdrs++;
		i++;
	}
	return (nbr_hdrs);
}

uint64_t		get_nbr_arena_headers(t_arena_list *list)
{
	uint64_t	nbr_hdrs;

	nbr_hdrs = 0;
	while (list)
	{
		nbr_hdrs += get_nbr_arena_hdrs_in_node(list);
		list = list->next;
	}
	return (nbr_hdrs);
}
