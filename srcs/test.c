/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 14:44:57 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/13 00:12:41 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ft_malloc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void print(char *s);
void print(char *s)
{
	write(1, s, strlen(s));
}
# define TINY_ALLOC 64
# define SMALL_ALLOC 663
# define LARGE_ALLOC 444

int main()
{
	char *addr;
	uint64_t	 i;

	i = 0;
	while (i < TINY_ALLOC)
		malloc(i++ % 64);
	i = 0;
	while (i < SMALL_ALLOC)
		malloc(i++ % 1024);
	i = 0;
	while (i < LARGE_ALLOC)
		malloc(i++ + 1042);

	show_alloc_mem();
/* 	i = 0; */
/* 	while (i < 1024) */
/* 	{ */
/* 		addr = malloc(1024); */
/* 		(void)addr; */
/* 		*addr = 0x2A; */
/* //		memset(addr, 42, 1024); */
/* 		i++; */
/* 	} */
/* 	show_alloc_mem(); */
	return (0);
}
