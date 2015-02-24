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
 
#include "dfexec.h"
#include "reusable_funcs.h"

/**
 * Create new task
 * @return pointer to the new task
 */
tpTask task_New()
{
	const int args_cnt = 16;
	tpTask tsk = (tpTask)xcalloc(1, sizeof(tTask));
	tsk->state = TSK_NOT_STARTED;
	tsk->os_params = 
		(struct __task_os_params*)xcalloc(1, sizeof(struct __task_os_params));
	tsk->avec = av_new();
	return tsk;
}

/**
 * Free all memory allocated for a task
 * @param tsk a task
 */
void task_Delete(tpTask tsk)
{
	av_delete(tsk->avec);
	xfree(tsk->os_params);
	xfree(tsk->path);
	xfree(tsk->work_dir);
	xfree(tsk->tstdin);
	xfree(tsk->tstdout);
	xfree(tsk->tstderr);
	xfree(tsk);
}

/**
 * Set executable file name of a task
 * @param tsk a task
 * @param path a filename
 */
void task_SetPath(tpTask tsk, char *path)
{
	xfree(tsk->path);
	tsk->path = xstrdup(path);
}

/**
 * Add argument to task command line arguments list
 * @param tsk a task
 * @param arg an argument to add
 */
void task_AddArg(tpTask tsk, char *arg)
{
	av_add_arg(tsk->avec, arg);
}

/**
 * Set working directory for a task
 * @param tsk a task
 * @param wdir a working directory name
 */
void task_SetWorkDir(tpTask tsk, char *wdir)
{
	xfree(tsk->work_dir);
	tsk->work_dir = xstrdup(wdir);
}

/**
 * Redirect task standard streams. When this task will be run, its file descriptors 0, 1 and
 * 2 will be redirected to the files. Use NULL to specify no redirection
 * @param tsk a task
 * @param stdin standard input filename
 * @param stdout standard output filename
 * @param stderr standard error filename
 */
void task_RedirectStdStreams(tpTask tsk, char *t_stdin, char *t_stdout, char *t_stderr)
{
	if ( stdin != NULL )
	{
		xfree(tsk->tstdin);
		tsk->tstdin = xstrdup(t_stdin);
	}
	if ( stdout != NULL )
	{
		xfree(tsk->tstdout);
		tsk->tstdout = xstrdup(t_stdout);
	}
	if ( stderr != NULL )
	{
		xfree(tsk->tstderr);
		tsk->tstderr = xstrdup(t_stderr);
	}
}

/**
 * Redirect task stdout to its stderr
 * @param tsk a task
 */
void task_DupStdoutToStderr(tpTask tsk)
{
	tsk->dup_stdout_to_stderr = 1;
}

/**
 * Set resource limits on a task including maximum real and CPU execution times, memory allocation
 * limit and maximum output file size. To set the limit use a value >= 0, to disable limit = 0
 * and to leave it untouched < 0
 * @param tsk task to set limits
 * @param real real time limit in milliseconds
 * @param cpu CPU time limit in milliseconds
 * @param mem memory allocation limit in bytes
 * @param out maximum output file size in bytes
 */
void task_SetLimits(tpTask tsk, int real, int cpu, int mem, int out)
{
	if ( real >= 0 )
	{
		tsk->rt_lim = real;
	}
	if ( cpu >= 0 )
	{
		tsk->cput_lim = cpu;
	}
	if ( mem >= 0 )
	{
		tsk->mem_lim = mem;
	}
	if ( out >= 0 )
	{
		tsk->out_lim = out;
	}
}
