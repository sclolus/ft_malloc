/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 14:44:57 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/10 01:12:07 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ft_malloc.h"


int	main(void)
{
	uint32_t	i;
	uint32_t	*ptr;
	uint32_t	size;

	PRINT(1, "Entered main\n");
	i = 0;
	size = 4096 * 6;
	while (i < 128) {
		/* PRINT(1, "\n"); */
		/* PRINT(1, ft_static_ulltoa(i)); */
		/* PRINT(1, "\n"); */
		if (!(ptr = malloc(size)))
			exit(EXIT_FAILURE);
		ft_memset(ptr, 42, size);
		i++;
	}
	return (0);
}
