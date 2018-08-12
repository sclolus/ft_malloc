/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_mutex.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/12 20:23:24 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/13 01:18:33 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

pthread_mutex_t	g_malloc_mutex;

void		malloc_lock_mutex(void)
{
	pthread_mutex_lock(&g_malloc_mutex);
}

void		malloc_unlock_mutex(void)
{
	pthread_mutex_unlock(&g_malloc_mutex);
}
