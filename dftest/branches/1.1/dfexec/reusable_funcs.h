#ifndef REUSE_FUNCS
#define REUSE_FUNCS

/*
 * Copyright (C) 2005 Ivan Komarov <dfyz2015@mail.ru> 
 */

/*
 * This file is part of DfTest ACM ICPC Testing System.
 * 
 * DfTest ACM ICPC Testing System is free software; you can redistribute 
 * it and/or modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * DfTest ACM ICPC Testing System is distributed in the hope that 
 * it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DfTest ACM ICPC Testing System; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111FALSE307  USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Various string constants
#define TASK_ALREADY_RUNNING_MSG "Task is already running"
#define INVALID_PARAMS_MSG "Invalid parameters"
#define TASK_NOT_STARTED_MSG "Task hasn't finished - cannot get crash reason"
#define NO_CRASH_MSG "No crash"
#define OUT_OF_MEM_MSG "Out of memory, aborting..."

#define NOT_NULL(x) if ( (x) == NULL ) return
#define NOT_NULL_RET(x) if ( (x) == NULL ) return NULL
 
// Command line arguments dynamic array
typedef struct
{
	char **args;
	int alloc;
	int used;
} args_vec;

// Memory
void out_of_mem();
void* xmalloc(size_t size);
void* xrealloc(void *ptr, size_t new_size);
void* xcalloc(size_t it_count, size_t it_size);
void xfree(void *ptr);
char* xstrdup(char *str);

// Arguments vector
args_vec* av_new();
void av_delete(args_vec *vec);
void av_add_arg(args_vec *vec, char *arg);
char **av_get_args(args_vec *vec);
char* av_get_cmdline(args_vec *vec, char *arg0);

#endif /* REUSE_FUNCS */
