/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 14:44:57 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/12 20:14:51 by sclolus          ###   ########.fr       */
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

int main()
{
	char *addr;

	if ((addr = malloc(~0UL)) != NULL)
		exit(EXIT_FAILURE);
	addr = malloc(16);
	free(NULL);
	free((char *)addr + 5);
	if (realloc((char *)addr + 5, 10) == NULL)
		print("Bonjours\n");
	void * ptr = malloc(1234);
	assert(*(uint8_t*)ptr == 0xaa);
//	free(ptr);
	free(ptr);
	return (0);
}
