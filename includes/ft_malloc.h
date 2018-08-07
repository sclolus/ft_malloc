/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 14:44:30 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/07 16:23:14 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MALLOC_H
# define FT_MALLOC_H

# include "libft.h"
# include <unistd.h>
# include <sys/mman.h>
# include <assert.h>

# include <stdio.h>//

/*
** Required functions
*/

extern void free(void *ptr);
extern void *malloc(size_t size);
extern void *realloc(void *ptr, size_t size);

# define ALLOCATIONS_PER_ARENA 128
# define DEFAULT_PAGE_SIZE 4096
# define TINY_ALLOCATION_SIZE 64
# define TINY_ARENA_SIZE TINY_ALLOCATION_SIZE * ALLOCATIONS_PER_ARENA
# define SMALL_ALLOCATION_SIZE DEFAULT_PAGE_SIZE
# define SMALL_ARENA_SIZE SMALL_ALLOCATION_SIZE * ALLOCATIONS_PER_ARENA

typedef enum	e_arena_type
{
	TINY_A = 0,
	SMALL_A,
	LARGE_A,
	SUPPORTED_ARENA_TYPE,
}				t_arena_type;

typedef struct	s_allocation_list t_allocation_list;

typedef uint64_t	t_alloc_bitmap;
typedef void		t_arena;

typedef struct	s_alloc_header
{
	uint64_t	size;
	uint64_t	md5_sum; // if I ever add this
}				t_alloc_header;

typedef enum	e_arena_state
{
	TRASHED = 0,
	USED,
}				t_arena_state;

typedef struct	s_arena_header // too large, please fix this
{
	/// the starting address of the arena
	void					*addr;
	uint64_t				nbr_pages;
	t_arena_type			t_arena_type;
	t_arena_state			state;
	/// the number of pages in the arena
	/// there should be exactly o;ne alloc_bitmap per page allocated
	t_alloc_bitmap			arena_alloc_bitmap[2];

	t_alloc_header			alloc_headers[128];
	uint32_t				alloc_number;
	uint8_t					pad[4];
}				t_arena_header;

t_arena_header	*allocate_arena(enum e_arena_type type);
t_arena_header	*add_new_arena(t_arena_list *list, t_arena_type type);


#define ARENA_LIST_SIZE_MULTIPLE 1

typedef struct	s_arena_list	t_arena_list;
typedef struct	s_arena_list
{
	t_arena_list		*prev;
	t_arena_header		*headers;
	uint64_t			nbr_arenas;
	uint64_t			capacity;
	t_arena_header		*last_trashed_arena_header;
	t_arena_list		*next;

}				t_arena_list;

t_arena_list	*remove_arena_list(t_arena_list *list);
t_arena_list	*add_arena_list(t_arena_list *list);
t_arena_list	*allocate_arena_list(void);
int				deallocate_arena_list(t_arena_list *list);
t_arena_list	*init_list(t_arena_list *list, uint64_t allocated_size);

t_arena_header	*find_first_unused_arena_header(t_arena_list *list);

typedef struct	s_malloc_info
{
	t_arena_list	*arena_lists[SUPPORTED_ARENA_TYPE];
	// arena sizes in number of memory pages
	uint64_t		arena_sizes[SUPPORTED_ARENA_TYPE];
	uint64_t		page_size;
	uint8_t			initialized;
	uint8_t			pad[7];
}				t_malloc_info;

extern t_malloc_info	g_malloc_info;


void	init_malloc_info(void);

#endif
