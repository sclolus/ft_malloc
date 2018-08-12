/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 14:44:57 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/12 19:16:54 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ft_malloc.h"


//# define TEST_SIZE 128 * 159

/* int	main(void) */
/* { */
/* 	uint32_t	i; */
/* 	uint32_t	*ptr; */
/* 	uint32_t	size; */
/* 	void		*ptrs[TEST_SIZE]; */

/* 	PRINT(1, "Entered main\n"); */
/* 	i = 0; */
/* 	size = 4096 * 6; */
/* 	while (i < TEST_SIZE) { */
/* 		/\* PRINT(1, "\n"); *\/ */
/* 		/\* PRINT(1, ft_static_ulltoa(i)); *\/ */
/* 		/\* PRINT(1, "\n"); *\/ */
/* 		if (!(ptr = malloc(size))) */
/* 			exit(EXIT_FAILURE); */
/* 		ft_memset(ptr, 42, size); */
/* 		ptrs[i] = ptr; */
/* 		i++; */
/* 	} */
/* 	i = 0; */
/* 	while (i< TEST_SIZE) */
/* 	{ */
/* 		free(ptrs[i]); */
/* 		i++; */
/* 	} */
/* 	return (0); */

/* } */
#include <stdlib.h>
#include <stdio.h>
/* int      main() */
/* { */
/* 	int   i; */
/* 	char  *addr; */
/* 	i = TEST_SIZE; */
/* 	i = 0; */
/* 	PRINT(1, "No allocation expected"); */
/* 	while (i < 1024) */
/* 	{ */
/* 		(void)addr; */
/* 		addr = (char*)malloc(1024); */
/* 		addr[0] = 42; */
/* //	  free(addr); */
/* 		i++; */
/* 	} */
/* 	return (0); */
/* } */

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
