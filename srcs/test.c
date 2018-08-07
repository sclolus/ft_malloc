/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 14:44:57 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/06 15:15:13 by sclolus          ###   ########.fr       */
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

	i = 0;
	size = 32;
	if (!(ptr = malloc(size * sizeof(uint32_t))))
		exit(EXIT_FAILURE);
	memset(ptr, 42, size * sizeof(uint32_t));
	return (0);
}
