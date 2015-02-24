/* Copyright (C) 2005 Ivan Komarov <dfyz2015@mail.ru> */

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

#include <string.h>

#define SECS_TO_MSECS 1000
#define MSECS_TO_NSECS 10000
#define SECS_TO_NSECS 10000000
#define KBYTES_TO_BYTES 1024

#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <psapi.h>
#include <string.h>

#define STREAM_REDIR_ERR_CODE 301
#define JOB_CREATE_ERR_CODE 302
#define JOB_PARAMS_ERR_CODE 303
#define CREATE_PROC_ERR_CODE 304
#define DLL_ERR_CODE 305

typedef BOOL (WINAPI *LogonFunction) (LPCWSTR,LPCWSTR,LPCWSTR,DWORD,LPCWSTR,LPWSTR,DWORD,
									  LPVOID,LPCWSTR,LPSTARTUPINFOW,LPPROCESS_INFORMATION);

void format_err_msg(char *buf, int size, int err_code, int os_err_code)
{
	const char *err_str;
	switch ( err_code )
	{
		case STREAM_REDIR_ERR_CODE:
			err_str = "Error while redirecting standard stream";
			break;
		case JOB_CREATE_ERR_CODE:
			err_str = "Win32 job error";
			break;
		case JOB_PARAMS_ERR_CODE:
			err_str = "Error while applying Win32 job parameters";
			break;
		case CREATE_PROC_ERR_CODE:
			err_str = "Process creation error";
			break;
		case DLL_ERR_CODE:
			err_str = "Dll error";
		default:
			err_str = "Unknown error";
	}
	_snprintf(buf, size, "%s: %s", err_str, strerror(os_err_code));
	buf[size-1] = '\0';
}

DLLEXPORT int task_GetExitCode(tpTask tsk)
{
	return tsk->os_params->exit_code;
}

DLLEXPORT int task_WasCrashed(tpTask tsk)
{
	return tsk->os_params->exit_code == 0;
}

DLLEXPORT void task_SetWin32Restrictions(tpTask tsk)
{
	tsk->restrictions = 1;
}

DLLEXPORT void task_RunAs(tpTask tsk, char *username, char *password, runas_type ra_type)
{
	tsk->os_params->username = username;
	tsk->os_params->password = password;
	tsk->os_params->ra_type = ra_type;
}

DLLEXPORT void task_GetCrashReason(tpTask tsk, char *reason, size_t size)
{
	char *s;
	if ( tsk->os_params->exit_code == 0 )
		_snprintf(reason, size, "%s", NO_CRASH_MSG);
	else if ( tsk->state != TSK_FINISHED )
		_snprintf(reason, size, "%s", TASK_NOT_STARTED_MSG);
	else
	{
		switch (tsk->os_params->exit_code)
		{
			case EXCEPTION_ACCESS_VIOLATION:
				s = "Access violation";
				break;
			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
				s = "Array bounds exceeded";
				break;
			case EXCEPTION_DATATYPE_MISALIGNMENT:
				s = "Datatype misaligment";
				break;
			case EXCEPTION_FLT_DENORMAL_OPERAND:
				s = "Too small operand in floating point expression";
				break;
			case EXCEPTION_FLT_DIVIDE_BY_ZERO:
				s = "Division by zero";
				break;
			case EXCEPTION_FLT_INEXACT_RESULT:
				s = "Inexact result in floating point expression";
				break;
			case EXCEPTION_FLT_INVALID_OPERATION:
				s = "Invalid floating point operation";
				break;
			case EXCEPTION_FLT_OVERFLOW:
				s = "Floating point overflow";
				break;
			case EXCEPTION_FLT_STACK_CHECK:
				s = "Stack over/underflow in floating point expression";
				break;
			case EXCEPTION_FLT_UNDERFLOW:
				s = "Floating point underflow";
				break;
			case EXCEPTION_ILLEGAL_INSTRUCTION:
				s = "Illegal processor instruction";
				break;
			case EXCEPTION_INT_DIVIDE_BY_ZERO:
				s = "Integer division by zero";
				break;
			case EXCEPTION_INT_OVERFLOW:
				s = "Integer overflow";
				break;
			case EXCEPTION_PRIV_INSTRUCTION:
				s = "Processor operation is not allowed";
				break;
			case EXCEPTION_STACK_OVERFLOW:
				s = "Stack overflow";
				break;
			default:
				s = "Unknown reason";
				break;
		}
		_snprintf(reason, size, "%s", s);
	}
	reason[size] = '\0';
}

DLLEXPORT int task_Run(tpTask tsk, task_run_result *rres, task_run_info *rinfo, char *err_msg, 
					   size_t err_msg_size)
{
	STARTUPINFO sinfo;
	STARTUPINFOW wstart;
	LogonFunction func;
	SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
	PROCESS_INFORMATION pinfo;
	HANDLE hin, hout, herr, hjob;
	JOBOBJECT_BASIC_UI_RESTRICTIONS ui = {0};
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION liminf = {0};
	__int64 freq, pstart, pcur;
	char* cmd;
	int ret = 1, err_code, exit_code;
	FILETIME start,exit,kernel;
	union
	{
		FILETIME ft;
		__int64 ticks;
	} user;
	PROCESS_MEMORY_COUNTERS meminfo;
	IO_COUNTERS count;
	int fdone = 0;

	*rres = RR_OK;
	if ( tsk->state == TSK_RUNNING )
	{
		_snprintf(err_msg, err_msg_size, TASK_ALREADY_RUNNING_MSG);
		return -1;
	}
	if ( tsk->path == NULL )
	{
		_snprintf(err_msg, err_msg_size, INVALID_PARAMS_MSG);
		return -1;
	}

	pinfo.hProcess = pinfo.hThread = INVALID_HANDLE_VALUE;
	memset(&sinfo,0,sizeof(sinfo));
	sinfo.dwFlags = STARTF_USESHOWWINDOW;

	hjob = INVALID_HANDLE_VALUE;
	hin = GetStdHandle(STD_INPUT_HANDLE);
	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	herr = GetStdHandle(STD_ERROR_HANDLE);
	if ( tsk->tstdin != NULL )
	{
		hin = CreateFile(tsk->tstdin, GENERIC_READ, 0, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		hout = CreateFile(tsk->tstdout, GENERIC_WRITE, FILE_SHARE_WRITE, &sa, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);
		herr = CreateFile(tsk->tstderr, GENERIC_WRITE, FILE_SHARE_WRITE, &sa, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);

		if ( hin == INVALID_HANDLE_VALUE || hout == INVALID_HANDLE_VALUE || herr == INVALID_HANDLE_VALUE )
		{
			format_err_msg(err_msg, err_msg_size, STREAM_REDIR_ERR_CODE, GetLastError());
			return FALSE;
		}
	}
	sinfo.hStdInput = hin;
	sinfo.hStdOutput = hout;
	sinfo.hStdError = hout;

	if ( tsk->restrictions )
	{
		hjob = CreateJobObject(NULL, NULL);
		if ( !hjob )
		{
			format_err_msg(err_msg, err_msg_size, JOB_CREATE_ERR_CODE, GetLastError());
			return -1;
		}
		liminf.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_ACTIVE_PROCESS | 
			JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION;
		liminf.BasicLimitInformation.ActiveProcessLimit = 1;
		ui.UIRestrictionsClass = JOB_OBJECT_UILIMIT_DESKTOP | JOB_OBJECT_UILIMIT_DISPLAYSETTINGS |
			JOB_OBJECT_UILIMIT_EXITWINDOWS | JOB_OBJECT_UILIMIT_READCLIPBOARD | 
			JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS | JOB_OBJECT_UILIMIT_WRITECLIPBOARD | JOB_OBJECT_UILIMIT_GLOBALATOMS
			| JOB_OBJECT_UILIMIT_HANDLES;
		if ( !SetInformationJobObject(hjob,JobObjectExtendedLimitInformation,&liminf,sizeof(liminf)) ||
			!SetInformationJobObject(hjob,JobObjectBasicUIRestrictions,&ui,sizeof(ui)) )
		{
			format_err_msg(err_msg, err_msg_size, JOB_PARAMS_ERR_CODE, GetLastError());
			return -1;
		}
	}
	
	cmd = av_get_cmdline(tsk->avec, tsk->path);
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);

	// We can't impersonate process with CreateProcessAsUser() due to lack of sufficient privileges.
	// Instead of that we have to deal with CreateProcessWithLogonW() function.
	// Notice:
	// 1) Win2000 lacks this function, therefore we must load it dynamically to compile succesfully on Win2000.
	// 2) All strings should be in Unicode, we convert them before function call.
	if ( tsk->os_params->username != NULL && tsk->os_params->password != NULL )
	{
		HINSTANCE hlib = LoadLibrary("advapi32.dll");
		if ( hlib )
		{
			func = (LogonFunction)GetProcAddress(hlib,"CreateProcessWithLogonW");
			if ( func )
			{
				int user_len = MultiByteToWideChar(CP_ACP, 0, tsk->os_params->username, FALSE, NULL, 0);
				int pass_len = MultiByteToWideChar(CP_ACP, 0, tsk->os_params->password, FALSE, NULL, 0);
				int cmd_len = MultiByteToWideChar(CP_ACP, 0, cmd, FALSE, NULL, 0);
				int wdir_len = MultiByteToWideChar(CP_ACP, 0, tsk->work_dir, FALSE, NULL, 0);
				PWSTR uw = (PWSTR)HeapAlloc(GetProcessHeap() , 0, user_len * sizeof(WCHAR));
				PWSTR pw = (PWSTR)HeapAlloc(GetProcessHeap(), 0 , pass_len * sizeof(WCHAR));
				PWSTR cw = (PWSTR)HeapAlloc(GetProcessHeap(), 0, cmd_len * sizeof(WCHAR));
				PWSTR ww = (PWSTR)HeapAlloc(GetProcessHeap(), 0, wdir_len * sizeof(WCHAR));
				MultiByteToWideChar(CP_ACP, 0, tsk->os_params->username, FALSE, uw, user_len);
				MultiByteToWideChar(CP_ACP, 0, tsk->os_params->password, FALSE, pw, pass_len);
				MultiByteToWideChar(CP_ACP, 0, cmd, FALSE, cw, cmd_len);
				MultiByteToWideChar(CP_ACP, 0, tsk->work_dir, FALSE, ww, wdir_len);
				wstart.hStdInput = sinfo.hStdInput;
				wstart.hStdOutput = sinfo.hStdOutput;
				wstart.hStdError = sinfo.hStdError;
				wstart.dwFlags = sinfo.dwFlags;
				ret = func(uw, L".", pw, 0, NULL, cw,
					CREATE_NEW_CONSOLE | CREATE_SUSPENDED, NULL, ww, &wstart, &pinfo);
				HeapFree(GetProcessHeap(), 0, uw);
				HeapFree(GetProcessHeap(), 0, pw);
				HeapFree(GetProcessHeap(), 0, cw);
				HeapFree(GetProcessHeap(), 0, ww);
				if ( !ret )
				{
					err_code = CREATE_PROC_ERR_CODE;
					ret = 0;
				}
			}
			else
			{
				err_code = DLL_ERR_CODE;
				ret = 0;
			}
			FreeLibrary(hlib);
		}
		else
		{
			err_code = DLL_ERR_CODE;
			ret = 0;
		}
	}
	else
	{
		ret = CreateProcess(NULL, cmd, NULL, NULL, TRUE, 
			CREATE_NO_WINDOW | CREATE_SUSPENDED, NULL, tsk->work_dir, &sinfo, &pinfo);
		if ( !ret )
		{
			err_code = CREATE_PROC_ERR_CODE;
			ret = 0;
		}
	}
	
	xfree(cmd);
	if ( !ret )
	{
		format_err_msg(err_msg, err_msg_size, err_code, GetLastError());
		return -1;
	}
	
	if ( tsk->restrictions )
	{
		AssignProcessToJobObject(hjob, pinfo.hProcess);
	}
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&pstart);
	ResumeThread(pinfo.hThread);
	while ( !fdone )
	{
		GetExitCodeProcess(pinfo.hProcess,&exit_code);
		if ( exit_code == STILL_ACTIVE )
		{
			GetProcessTimes(pinfo.hProcess,&start,&exit,&kernel,&user.ft);
			QueryPerformanceCounter((LARGE_INTEGER*)&pcur);
			GetProcessMemoryInfo(pinfo.hProcess,&meminfo,sizeof(meminfo));
			GetProcessIoCounters(pinfo.hProcess,&count);
			if ( tsk->cput_lim != 0 && user.ticks > (tsk->cput_lim * MSECS_TO_NSECS) )
			{
				*rres = RR_CPU_TIME_LIMIT;
			}
			if ( tsk->rt_lim != 0 && 
				(__int64)(((double)(pcur - pstart) / (double)freq) * SECS_TO_NSECS) > 
				(tsk->rt_lim * MSECS_TO_NSECS) )
			{
				*rres = RR_REAL_TIME_LIMIT;
			}
			if ( tsk->mem_lim != 0 && 
				(int)meminfo.PeakWorkingSetSize > tsk->mem_lim )
			{
				*rres = RR_MEM_LIMIT;
			}
			if ( tsk->out_lim != 0 &&
				count.WriteTransferCount > tsk->out_lim )		
			{
				*rres = RR_OUTPUT_LIMIT;
			}
			if ( (*rres) != RR_OK )
			{
				if ( tsk->restrictions )
					TerminateJobObject(hjob,1);
				else
					TerminateProcess(pinfo.hProcess, 1);
				fdone = 1;
			}
			else
				Sleep(TIME_ATOM);
		}
		else
			fdone = 1;
	}

	GetProcessTimes(pinfo.hProcess,&start,&exit,&kernel,&user.ft);
	GetProcessMemoryInfo(pinfo.hProcess,&meminfo,sizeof(meminfo));
	GetProcessIoCounters(pinfo.hProcess,&count);

	rinfo->time_worked = (int)((double)user.ticks / SECS_TO_MSECS);
	rinfo->memory_used = meminfo.PeakWorkingSetSize;
	rinfo->output_size = (int)count.WriteTransferCount;
	GetExitCodeProcess(pinfo.hProcess, &tsk->os_params->exit_code);
	if ( tsk->os_params->exit_code != 0 )
		*rres = RR_CRASH;

	CloseHandle(pinfo.hProcess);
	CloseHandle(pinfo.hThread);
	if ( hin != INVALID_HANDLE_VALUE && hin != GetStdHandle(STD_INPUT_HANDLE) )
		CloseHandle(hin);
	if ( hout != INVALID_HANDLE_VALUE && hout != GetStdHandle(STD_OUTPUT_HANDLE) )
		CloseHandle(hout);
	if ( herr != INVALID_HANDLE_VALUE && herr != GetStdHandle(STD_ERROR_HANDLE) )
		CloseHandle(herr);
	SetErrorMode(0);
	return 0;
}
