/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sclolus <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/06 14:44:30 by sclolus           #+#    #+#             */
/*   Updated: 2018/08/13 01:33:54 by sclolus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MALLOC_H
# define FT_MALLOC_H

# include <pthread.h>
# include "libft.h"
# include <unistd.h>
# include <sys/mman.h>
# include <fcntl.h>

/*
** Required functions
*/

extern void		free(void *ptr);
extern void		*malloc(size_t size);
extern void		*realloc(void *ptr, size_t size);
extern void		*reallocf(void *ptr, size_t size);
extern void		*calloc(size_t count, size_t size);
extern void		*valloc(size_t size);
extern void		show_alloc_mem(void);

# define ALLOCATIONS_PER_ARENA 128
# define DEFAULT_PAGE_SIZE 1024
# define TINY_ALLOCATION_SIZE 64
# define TINY_ARENA_SIZE TINY_ALLOCATION_SIZE * ALLOCATIONS_PER_ARENA
# define SMALL_ALLOCATION_SIZE DEFAULT_PAGE_SIZE
# define SMALL_ARENA_SIZE SMALL_ALLOCATION_SIZE * ALLOCATIONS_PER_ARENA

# define HEX_BASE "0123456789abcdef"
# define CONSTRUCTOR __attribute__((constructor(99999))) void

typedef enum	e_arena_type
{
	TINY_A = 0,
	SMALL_A,
	LARGE_A,
	SUPPORTED_ARENA_TYPES,
}				t_arena_type;

typedef struct s_allocation_list	t_allocation_list;

typedef uint64_t	t_alloc_bitmap;
typedef void		t_arena;

typedef struct	s_alloc_header
{
	uint16_t	size;
}				t_alloc_header;

typedef enum	e_arena_state
{
	TRASHED = 0,
	USED,
}				t_arena_state;

typedef struct	s_arena_header
{
	void					*addr;
	uint64_t				nbr_pages;
	t_arena_type			arena_type;
	t_arena_state			state;
	t_alloc_bitmap			arena_alloc_bitmap[2];
	t_alloc_header			alloc_headers[ALLOCATIONS_PER_ARENA];
	uint32_t				alloc_number;
	uint8_t					pad[4];
}				t_arena_header;

# define ARENA_LIST_SIZE_MULTIPLE 1

typedef struct	s_arena_list
{
	struct s_arena_list		*prev;
	t_arena_header			*headers;
	uint64_t				nbr_arenas;
	uint64_t				capacity;
	t_arena_header			*last_trashed_arena_header;
	struct s_arena_list		*next;
}				t_arena_list;

t_arena_type	get_arena_type_by_size(size_t size);
t_arena			*allocate_arena(uint64_t nbr_pages);
int32_t			deallocate_arena(t_arena *arena, uint64_t len);
t_arena_header	*add_new_arena(t_arena_list *list,
								t_arena_type type,
								uint64_t size);
void			*allocate_memory_on_arena(t_arena_header *hdr, uint64_t size);
t_arena_header	*find_first_available_arena(t_arena_list *list);
int32_t			is_addr_allocated_in_arena(void *addr, t_arena_header *hdr);
t_arena_list	*trash_arena(t_arena_header *hdr, t_arena_list *node);

void			*malloc_on_arenas(uint64_t size,
								t_arena_list *list,
								t_arena_type type);
void			*realloc_on_arenas(uint64_t size,
									t_arena_list *list,
									t_arena_type type,
									void *ptr);
void			free_memory_zone(void *addr, t_arena_list *node);

t_arena_list	*remove_arena_list(t_arena_list *list);
t_arena_list	*add_arena_list(t_arena_list *list);
t_arena_list	*allocate_arena_list(void);
int				deallocate_arena_list(t_arena_list *list);
t_arena_list	*init_list(t_arena_list *list, uint64_t allocated_size);

t_arena_header	*find_first_unused_arena_header(t_arena_list *node);
t_arena_list	*find_first_available_arena_list(t_arena_list *list);
t_arena_list	*find_addr_in_arenas(void *addr);
t_arena_list	*find_addr_in_arena_list(void *addr, t_arena_list *list);
t_arena_header	*find_addr_in_hdr_list(void *addr, t_arena_list *list);

typedef struct	s_arena_type_info
{
	uint64_t			allocation_per_arena;
	uint64_t			nbr_pages;
	uint64_t			allocation_size;
	t_arena_type		type;
	uint8_t				pad[4];
}				t_arena_type_info;

typedef struct	s_malloc_flags
{
	uint16_t	log_file : 1;
	uint16_t	scribble : 1;
	uint16_t	error_abort : 1;
	uint16_t	pad : 13;
}				t_malloc_flags;

typedef struct	s_malloc_info
{
	t_arena_list		*arena_lists[SUPPORTED_ARENA_TYPES];
	t_arena_type_info	arena_type_infos[SUPPORTED_ARENA_TYPES];
	uint64_t			page_size;
	int					fd_output;
	t_malloc_flags		flags;
	uint8_t				initialized;
	uint8_t				main_was_called;
}				t_malloc_info;

extern t_malloc_info	g_malloc_info;
extern pthread_mutex_t	g_malloc_mutex;

void			main_was_called(void);

int32_t			init_malloc_info(void);

void			malloc_lock_mutex(void);
void			malloc_unlock_mutex(void);
void			print_mem_arenas(t_arena_header **hdrs,
							uint64_t nbr_hdrs);
uint64_t		get_nbr_arena_headers(t_arena_list *list);

# define PRINT(fd, x) write(fd, x, strlen(x));

#endif
