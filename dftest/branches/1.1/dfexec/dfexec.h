#ifndef DFEXEC_H
#define DFEXEC_H

/* Copyright (C) 2005 Ivan Komarov <dfyz2015@mail.ru> */
/* Uses TinyXml XML parsing library, copyright (c) 2000-2002 Lee Thomason 
(www.grinninglizard.com) */

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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#if !defined WIN32 && !defined __linux__
#error "Execlib won't compile on this operating system"
#endif

#include "reusable_funcs.h"

#define TIME_ATOM 20

#if defined WIN32
#define DLLEXPORT __declspec (dllexport)
#else
#define DLLEXPORT
#endif

typedef enum 
{
	RR_OK, // Terminated OK
	RR_CPU_TIME_LIMIT, // CPU time limit
	RR_REAL_TIME_LIMIT, // Real time limit
	RR_MEM_LIMIT, // Memory allocation limit
	RR_OUTPUT_LIMIT, // Output file size limit
	RR_CRASH // Terminated abnormally (crashed)
} task_run_result;

typedef enum
{
	TSK_NOT_STARTED,
	TSK_RUNNING,
	TSK_FINISHED
} task_state;

typedef struct
{
	int time_worked; // CPU time used by program, in milliseconds
	int memory_used; // Memory allocated by program, in bytes
	int output_size; // Bytes written by program, in bytes
} task_run_info;

typedef enum
{
	RAT_UNPRIVILGED,
	RAT_PRIVILEGED
} runas_type;

#ifdef WIN32
struct __task_os_params
{
	char *username, *password;
	runas_type ra_type;
	int exit_code;
};
#endif

#ifdef __linux__
struct __task_os_params
{};
#endif

struct __task
{
	char *path; // Task executable filename
	args_vec *avec;
	char *work_dir; // Task working directory
	char *tstdin;
	char *tstdout;
	char *tstderr; // Filenames to redirect standard streams
	int dup_stdout_to_stderr; // Should stdout fd be used as stderr fd?
	int rt_lim, cput_lim; // Real and CPU time limits for the task
	int mem_lim; // Memory allocation limit for the task
	int out_lim; // Maximum amount of bytes the task can write
	int restrictions; // Restrictions applied to process
	task_state state; // Task current state
	struct __task_os_params *os_params; // OS dependent parameters
};

typedef struct __task tTask, *tpTask;

// Functions in dftest-common.c
DLLEXPORT tpTask task_New();
DLLEXPORT void task_Delete(tpTask);

DLLEXPORT void task_SetPath(tpTask, char*);
DLLEXPORT void task_AddArg(tpTask, char*);
DLLEXPORT void task_SetWorkDir(tpTask, char*);

DLLEXPORT void task_RedirectStdStreams(tpTask, char*, char*, char*);
DLLEXPORT void task_DupStdoutToStderr(tpTask);

DLLEXPORT void task_SetLimits(tpTask, int, int, int, int);
// End of functions in dftest-common.c

// Platform-dependent functions
DLLEXPORT int task_GetExitCode(tpTask);
DLLEXPORT int task_WasCrashed(tpTask);
DLLEXPORT void task_GetCrashReason(tpTask, char*, size_t);

DLLEXPORT int task_Run(tpTask, task_run_result*, task_run_info*, char*, size_t);

DLLEXPORT void task_SetUnixRestrictions(tpTask, int);
DLLEXPORT int task_GetUnixRestrictions(tpTask);

DLLEXPORT void task_RunAs(tpTask, char*, char*, runas_type);
DLLEXPORT void task_SetWin32Restrictions(tpTask);
// End of platform-dependent functions

#endif

