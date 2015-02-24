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

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#define HARD_MEM_LIM (16 * 1024 * 1024)

#define FORK_ERR_CODE 300
#define DUP2_ERR_CODE 301
#define OPEN_ERR_CODE 302
#define CHDIR_ERR_CODE 303
#define PROCFS_ERR_CODE 304
#define PIPE_ERR_CODE 305
#define FCNTL_ERR_CODE 306
#define SETRLIMIT_ERR_CODE 307

#define RS_DROP_CAPS 0x1
#define RS_NO_CORE 0x2
#define RS_FDS 0x4
#define RS_CHROOT 0x8
#define RS_PROCS 0x10

struct __task_os_params
{
	int pid;
	int status;
};

/**
 * Apply a UNIX resctrictions to a task. Look through RS_* #defines to see all possibilities
 * @param tsk a task
 * @param restrictions restriction flags
 */
void task_SetUnixRestrictions(tpTask tsk, int restrictions)
{
	tsk->restrictions = restrictions;
}

/**
 * Get the current UNIX restrictions of a task.
 * @param tsk a task
 * @return restriction flags
 */
int task_GetUnixRestrictions(tpTask tsk)
{
	return tsk->restrictions;
}

/**
 * Return exit code of the task
 * @param tsk task to check exit code
 * @return task exit code
 */
int task_GetExitCode(tpTask tsk)
{
	if ( tsk->state != TSK_FINISHED || !WIFEXITED(tsk->os_params->status) )
	{
		return -1;
	}
	return WEXITSTATUS(tsk->os_params->status);
}

/**
 * Check if task was terminated abnormally
 * @param tsk task to check
 * @return >0 in case of abnormal termination, 0 in case of normal termination and -1 if task
 * hasn't finshed
 */
int task_WasCrashed(tpTask tsk)
{
	if ( tsk->state != TSK_FINISHED )
	{
		return -1;
	}
	return WIFSIGNALED(tsk->os_params->status);
}

/**
 * If task was terminated abnormally (by a signal), print a message describing crash reason
 * @param tsk task which was termiated
 * @param reason a buffer where a message will be printed to
 * @param size maximum size of a message
 */
void task_GetCrashReason(tpTask tsk, char *reason, size_t size)
{
	char *s;
	if ( tsk->state != TSK_FINISHED )
	{
		snprint(reason, size, "Task hasn't finished: cannot get crash reason");
	}
	else if ( !WIFSIGNALED(tsk->os_params->status) )
	{
		snprintf(reason, size, "No crash");
	}
	else
	{
		switch ( WTERMSIG(tsk->os_params->status) )
		{
			case SIGABRT:
				s = "Aborted (SIGABRT, SIGIOT)";	
				break;
			case SIGALRM: case SIGVTALRM:
				s = "Killed by alarm (SIGALRM, SIGVTALRM)";
				break;
			case SIGBUS:
				s = "Bus error (SIGBUS)";
				break;
			case SIGFPE:
				s = "Floating point exception (SIGFPE)";
				break;
			case SIGHUP:
				s = "Hangup (SIGHUP)";
				break;
			case SIGILL:
				s = "Illegal instruction (SIGILL)";
				break;
			case SIGINT:
				s = "Interrupted (SIGINT)";
				break;
			case SIGKILL:
				s = "Killed (SIGKILL)";
				break;
			case SIGPIPE:
				s = "Broken pipe (SIGPIPE)";
				break;
			case SIGQUIT:
				s = "Quit signal (SIGQUIT)";
				break;
			case SIGSEGV:
				s = "Segmentation violation (SIGSEGV)";
				break;
			case SIGTERM:
				s = "Terminated (SIGTERM)";
				break;
			case SIGUSR1: case SIGUSR2:
				s = "User signal (SIGUSR1, SIGUSR2)";
				break;
			case SIGPOLL: 
				s = "I/O is now possible (SIGPOLL, SIGIO)";
				break;
			case SIGPROF:
				s = "Profiling timer expired (SIGPROF)";
				break;
			case SIGSYS:
				s = "Bad syscall (SIGSYS)";
				break;
			case SIGTRAP:
				s = "Trace trap (SIGTRAP)";
				break;
			default:
				s = "Unknown reason";
				break;
				
		}
		snprintf(reason, size, "%s", s);
	}
	reason[size-1] = '\0';
}

/**
 * Formats pretty message describing error in task_Run()
 * @param buf a buffer to print
 * @param size maximun size of buffer
 * @param err_code error code (defined above)
 * @param os_err_code errno number
 */
void format_err_msg(char *buf, int size, int err_code, int os_err_code)
{
	const char *err_str;
	switch ( err_code )
	{
		case DUP2_ERR_CODE:
			err_str = "dup2() failed";
			break;
		case FORK_ERR_CODE:
			err_str = "fork() failed";
			break;
		case OPEN_ERR_CODE:
			err_str = "open() failed";
			break;
		case CHDIR_ERR_CODE:
			err_str = "chdir() failed";
			break;
		case PROCFS_ERR_CODE:
			err_str = "Cannot read information from /proc filesystem";
			break;
		case PIPE_ERR_CODE:
			err_str = "pipe() failed";
			break;
		default:
			err_str = "Unknown error";
	}
	snprintf(buf, size, "%s: %s", err_str, strerror(os_err_code));
	buf[size-1] = '\0';
}

// Struct used to communicate between child and parent
typedef struct
{
	int main_code;
	int errno_code;
} comm_msg;

/**
 * Run a task applying all limits and restrictions.
 * @param tsk a task to run
 * @param rres task run result
 * @param rinfo this structure will contain task run info (used time, memory and output size)
 * @param err_msg in case of failure, error message will be printed to this buffer
 * @param err_msg_size maximum size of buffer
 * @return 0 if task run was succeded, -1 otherwise
 */
int task_Run(tpTask tsk, task_run_result *rres, task_run_info *rinfo, char *err_msg, 
	     size_t err_msg_size)
{
	int comm_fds[2];
	comm_msg msg;
	int br;

	if ( tsk->state == TSK_RUNNING )
	{
		snprintf(err_msg, err_msg_size, "The task is already running");
	}
	if ( tsk->path == NULL )
	{
		snprintf(err_msg, err_msg_size, "Path wasn't specified");
		return -1;
	}
	
	memset(&msg, 0, sizeof(msg));
	*rres = RR_OK;
	if ( pipe(comm_fds) == -1 )
	{
		format_err_msg(err_msg, err_msg_size, PIPE_ERR_CODE, errno);
		return -1;
	}
	if ( fcntl(comm_fds[1], F_SETFD, FD_CLOEXEC) == -1 )
	{
		format_err_msg(err_msg, err_msg_size, FCNTL_ERR_CODE, errno);
	}
	tsk->os_params->pid = fork();
	switch ( tsk->os_params->pid )
	{
		case -1:
			format_err_msg(err_msg, err_msg_size, FORK_ERR_CODE, errno);
			return -1;
		case 0:
			close(comm_fds[1]);
			br = read(comm_fds[0], &msg, sizeof(comm_msg));
			if ( br == -1 || br != sizeof(comm_msg) )
			{
				return -1;
			}
			if ( msg.main_code != 0 )
			{
				format_err_msg(err_msg, err_msg_size, msg.main_code, msg.errno_code);
				return -1;
			}
			break;
		default:
			close(comm_fds[0]);
			// Redirect standard streams
			if ( tsk->stdin != NULL )
			{
				int stdin_fd = open(tsk->stdin, O_RDONLY);
				if ( stdin_fd == -1 )
				{
					msg.main_code = OPEN_ERR_CODE;
					msg.errno_code = errno;
					write(comm_fds[1], &msg, sizeof(msg));
					_exit(1);
				}
				if ( dup2(stdin_fd, STDIN_FILENO) == -1 )
				{
					msg.main_code = DUP2_ERR_CODE;
					msg.errno_code = errno;
					write(comm_fds[1], &msg, sizeof(msg));
					_exit(1);
				}
				close(stdin_fd);
			}
			if ( tsk->stdout != NULL )
			{
				int stdout_fd = open(tsk->stdout, O_WRONLY|O_CREAT, S_IRWXO|S_IWOTH);
				if ( stdout_fd == -1 )
				{
					msg.main_code = OPEN_ERR_CODE;
					msg.errno_code = errno;
					write(comm_fds[1], &msg, sizeof(msg));
					_exit(1);
				}
				if ( dup2(stdout_fd, STDOUT_FILENO ) == -1 )
				{
					msg.main_code = DUP2_ERR_CODE;
					msg.errno_code = errno;
					write(comm_fds[1], &msg, sizeof(msg));
					_exit(1);
				}
				close(stdout_fd);
				if ( tsk->dup_stdout_to_stderr && dup2(STDOUT_FILENO, STDERR_FILENO) == -1 )
				{
					msg.main_code = DUP2_ERR_CODE;
					msg.errno_code = errno;
					write(comm_fds[1], &msg, sizeof(msg));
					_exit(1);
				}
				else if ( tsk->stderr != NULL )
				{
					int stderr_fd = open(tsk->stderr, O_WRONLY|O_CREAT, S_IRWXO|S_IWOTH);
					if ( stderr_fd == -1 )
					{
						
					}
				}
			}
			// Apply restrictions
			if ( tsk->restrictions & RS_NO_CORE )
			{
				struct rlimit r;
				memset(&r, 0, sizeof(r));
				if ( setrlimit(RLIMIT_CORE, &r) == -1 )
				{
					msg.main_code = SETRLIMIT_ERR_CODE;
					msg.errno_code = errno;
					write(comm_fds[1], &msg, sizeof(msg));
					_exit(1);
				}
			}
			if ( tsk->restrictions & RS_PROCS )
			{
				struct rlimit r;
				memset(&r, 0, sizeof(r));
				if ( setrlimit(RLIMIT_NPROC, &r) == -1 )
				{
					msg.main_code = SETRLIMIT_ERR_CODE;
					msg.errno_code = errno;
					write(comm_fds[1], &msg, sizeof(msg));
					_exit(1);
				}
			}
			if ( tsk->restrictions & RS_CHROOT)
			{}
			
			break;
	}
}

/*


/*uint64 update_mem_usage(FILE* file_desc)
{
	uint64 ret_val;
	if ( fseek(file_desc, 0, SEEK_SET) == -1 )
	{
		return -1;
	}
	if ( fscanf(file_desc, "%d", &ret_val) != 1 )
	{
		return -1;
	}
	return ret_val * getpagesize();
}

FILE* get_fd_by_pid(pid_t pid)
{
	char buffer[30];
	snprintf(buffer, sizeof(buffer), "%d", pid);
	return fopen(buffer, "r");
}

void create_argv (char *args_str, char *filename, char **args_arr)
{
	args_arr[0] = filename;
	int ind = 1;
	args_arr[ind] = strtok (args_str, " ");
	while (args_arr[ind++] != NULL && ind < MAX_ARGS)
	{
		args_arr[ind] = strtok (NULL, ARGS_DELIMS);
	}
}

DLLEXPORT const char* get_crash_by_code (int code)
{
	return "";
}

DLLEXPORT int run_program (enum run_result *res, struct run_program_info *info, int *exit_code, char *err_msg, 
			   int err_msg_size)
{
	int pid;
	int ch_term = FALSE;
	
	int comm_fds[2];
	int child_status;
	char *args[MAX_ARGS];
	int args_ind = 0;
	int args_len = 4;
	FILE *statm;
	struct rusage ch_usage;

	if (info->filename == NULL || info->arguments == NULL || res == NULL )
	{
		snprintf(err_msg, err_msg_size, "Invalid parameters");
		err_msg[err_msg_size-1] = '\0';
		return FALSE;
	}
	
	*res = rr_success;
	create_argv (info->arguments, info->filename, args);
	if ( info->work_dir != NULL && chdir(info->work_dir) == -1 )
	{
		format_err_msg(err_msg, err_msg_size, CHDIR_ERR_CODE, errno);
		return FALSE;
	}
	
	pid = fork ();
	switch (pid)
	{
	case -1:
		format_err_msg(err_msg, err_msg_size, FORK_ERR_CODE, errno);
		return FALSE;
	case 0:
		if ( info->redirects != NULL )
		{
			int in_fd, out_fd, err_fd;
			if ( info->redirects->stdin != NULL )
			{
				in_fd = open(info->redirects->stdin, O_RDONLY);
				if ( in_fd == -1 )
				{
					err_msg = OPEN_ERR_MSG;
					return FALSE;
				}
				if ( dup2(in_fd, STDIN_FILENO) == -1 )
				{
					err_msg = DUP_ERR_MSG;
					return FALSE;
				}
			}
			if ( info->redirects->stdout != NULL )
			{
				out_fd = open(info->redirects->stdout, O_WRONLY | O_CREAT);
				if ( out_fd == -1 )
				{
					err_msg = OPEN_ERR_MSG;
					return FALSE;
				}
				if ( dup2(out_fd, STDOUT_FILENO) == -1 )
				{
					err_msg = DUP_ERR_MSG;
					return FALSE;
				}
			}
			if ( info->redirects->stderr != NULL )
			{
				err_fd = open(info->redirects->stderr, O_WRONLY | O_CREAT);
				if ( err_fd == -1 )
				{
					err_msg = OPEN_ERR_MSG;
					return FALSE;
				}
				if ( dup2(err_fd, STDERR_FILENO) == -1 )
				{
					err_msg = DUP_ERR_MSG;
					return FALSE;
				}
			}
		}
		execvp(info->filename, args);
		break;
	default:
		while ( !ch_term )
		{
			if ( wait4( pid, &child_status, WNOHANG, &ch_usage ) == 0  )
			{
				ch_term = TRUE;
			}
			else
			{
				ch_term = TRUE;
				if ( WIFEXITED(child_status) )
				{
					*exit_code = WEXITSTATUS(child_status);
				}
				else
				{
					
				}
			}
		}
		break;
	}
}
*/
