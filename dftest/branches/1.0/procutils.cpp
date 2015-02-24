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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#include "procutils.hpp"

CProcess::CProcess(std::string filename, std::string arguments, std::string working_directory)
{
	crashed = false;
#ifdef WIN32
	file = filename;
	args = arguments;
	work_dir = working_directory;
	
	pinfo.hProcess = INVALID_HANDLE_VALUE;
	pinfo.hThread = INVALID_HANDLE_VALUE;
	memset(&info,'\0',sizeof(STARTUPINFO));

	info.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	info.wShowWindow = SW_HIDE;
	info.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	info.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	info.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	hin = hout = herr = INVALID_HANDLE_VALUE;

	hjob = CreateJobObject(NULL,NULL);
	if ( !hjob )
	{
		throw DftestException("Cannot create WIN32 job object", true);
	}
	JOBOBJECT_BASIC_LIMIT_INFORMATION basic = {0};
	basic.LimitFlags = JOB_OBJECT_LIMIT_ACTIVE_PROCESS;
	basic.ActiveProcessLimit = 1;
	if ( !SetInformationJobObject(hjob,JobObjectBasicLimitInformation,&basic,sizeof(basic)) )
	{
		throw DftestException("Cannot apply WIN32 job limits", true);
	}
	JOBOBJECT_BASIC_UI_RESTRICTIONS ui = {0};
	ui.UIRestrictionsClass = JOB_OBJECT_UILIMIT_DESKTOP | JOB_OBJECT_UILIMIT_DISPLAYSETTINGS |
		JOB_OBJECT_UILIMIT_EXITWINDOWS | JOB_OBJECT_UILIMIT_READCLIPBOARD | 
		JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS | JOB_OBJECT_UILIMIT_WRITECLIPBOARD | JOB_OBJECT_UILIMIT_GLOBALATOMS
		| JOB_OBJECT_UILIMIT_HANDLES;
	BOOL r = SetInformationJobObject(hjob,JobObjectBasicUIRestrictions,&ui,sizeof(ui));
	
	hport = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);
	if ( !hport )
	{
		throw DftestException("Cannot create WIN32 completition port", true);
	}
	JOBOBJECT_ASSOCIATE_COMPLETION_PORT assoc = {0};
	assoc.CompletionKey = (PVOID)1;
	assoc.CompletionPort = hport;
	if ( !SetInformationJobObject(hjob,JobObjectAssociateCompletionPortInformation,&assoc,sizeof(assoc)) )
	{
		throw DftestException("Cannot associate WIN32 completition port with WIN32 job", true);
	}
#endif
}

CProcess::~CProcess()
{
#ifdef WIN32
	Clean();
#endif
}

void CProcess::Clean()
{
#ifdef WIN32
	if ( pinfo.hProcess != INVALID_HANDLE_VALUE )
	{
		CloseHandle(pinfo.hProcess);
		pinfo.hProcess = INVALID_HANDLE_VALUE;
	}
	if ( pinfo.hThread != INVALID_HANDLE_VALUE )
	{
		CloseHandle(pinfo.hThread);
		pinfo.hThread = INVALID_HANDLE_VALUE;
	}
	if ( hin != INVALID_HANDLE_VALUE && hin != GetStdHandle(STD_INPUT_HANDLE) )
	{
		CloseHandle(hin);
		hin = INVALID_HANDLE_VALUE;
	}
	if ( hout != INVALID_HANDLE_VALUE && hout != GetStdHandle(STD_OUTPUT_HANDLE) )
	{
		CloseHandle(hout);
		hout = INVALID_HANDLE_VALUE;
	}
	if ( herr != INVALID_HANDLE_VALUE && herr != GetStdHandle(STD_ERROR_HANDLE) )
	{
		CloseHandle(herr);
		herr = INVALID_HANDLE_VALUE;
	}
#endif
}

bool CProcess::Crashed()
{
	return crashed;
}

void CProcess::RunAs(std::string usr, std::string passwd)
{
	if ( usr == "" )
	{
		throw DftestException("User name cannot be empty", false);
	}
	else
	{
		username = usr;
		password = passwd;
	}
}

bool CProcess::StartWithTL(unsigned int time_limit)
{
#ifdef WIN32
	std::string cmd = file + " " + args;
	bool ret = true;

	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
	if ( !CreateProcess(NULL, (char*)cmd.c_str(), NULL, NULL, TRUE, 
		CREATE_NO_WINDOW, NULL, work_dir.c_str(), &info, &pinfo) )
	{
		throw DftestException("Failed to create process " + GetBasename(file), true);
	}
	
	if ( WaitForSingleObject(pinfo.hProcess, time_limit * SECS_TO_MSECS) != WAIT_OBJECT_0 ) 
	{
		TerminateProcess(pinfo.hProcess,1);
		ret = false;
	}

	GetExitCodeProcess(pinfo.hProcess,&exit_code);
	SetErrorMode(0);
	Clean();
	return ret;
#endif
}

#ifdef WIN32
void CheckForCrash()
{
	
}
#endif

RunResult CProcess::StartWithAllLimits(RunInfo* r_info)
{
#ifdef WIN32
	std::string cmd = file + " " + args;
	RunResult res = rrSuccess;

	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
	if ( username != "" )
	{
		HINSTANCE hlib = LoadLibrary("advapi32.dll");
		if ( !hlib )
		{
			throw DftestException("Cannot load advapi32.dll library dll", true);
		}
		LogonFunction func = (LogonFunction)GetProcAddress(hlib,"CreateProcessWithLogonW");
		if ( func )
		{
			int user_len = MultiByteToWideChar(CP_ACP,0,username.c_str(),-1,NULL,0);
			int pass_len = MultiByteToWideChar(CP_ACP,0,password.c_str(),-1,NULL,0);
			int cmd_len = MultiByteToWideChar(CP_ACP,0,cmd.c_str(),-1,NULL,0);
			int wdir_len = MultiByteToWideChar(CP_ACP,0,work_dir.c_str(),-1,NULL,0);
			PWSTR uw = (PWSTR)HeapAlloc(GetProcessHeap(),0,user_len * sizeof(WCHAR));
			PWSTR pw = (PWSTR)HeapAlloc(GetProcessHeap(),0,pass_len * sizeof(WCHAR));
			PWSTR cw = (PWSTR)HeapAlloc(GetProcessHeap(),0,cmd_len * sizeof(WCHAR));
			PWSTR ww = (PWSTR)HeapAlloc(GetProcessHeap(),0,wdir_len * sizeof(WCHAR));
			MultiByteToWideChar(CP_ACP, 0, username.c_str(), -1, uw, user_len);
			MultiByteToWideChar(CP_ACP, 0, password.c_str(), -1, pw, pass_len);
			MultiByteToWideChar(CP_ACP, 0, cmd.c_str(), -1, cw, cmd_len);
			MultiByteToWideChar(CP_ACP, 0, work_dir.c_str(), -1, ww, wdir_len);
			STARTUPINFOW wstart = {0};
			wstart.hStdInput = info.hStdInput;
			wstart.hStdOutput = info.hStdOutput;
			wstart.hStdError = info.hStdError;
			wstart.dwFlags = info.dwFlags;
			BOOL ok = func(uw, L".", pw, 0, NULL, cw,
				CREATE_NEW_CONSOLE | CREATE_SUSPENDED, NULL, ww, &wstart, &pinfo);
			HeapFree(GetProcessHeap(),0,uw);
			HeapFree(GetProcessHeap(),0,pw);
			HeapFree(GetProcessHeap(),0,cw);
			HeapFree(GetProcessHeap(),0,ww);
			if ( !ok )
			{
				throw DftestException("Failed to create process " + GetBasename(file) 
					+ " as user " + username, true);
			}
		}
		FreeLibrary(hlib);
		if ( !func )
		{
			throw DftestException("Cannot get CreateProcessWithLogonW() address", true);
		}
	}
	else
	{
		if ( !CreateProcess(NULL, (char*)cmd.c_str(), NULL, NULL, TRUE, 
		CREATE_NO_WINDOW | CREATE_SUSPENDED, NULL, work_dir.c_str(), &info, &pinfo) )
		{
			throw DftestException("Failed to create process " + GetBasename(file), true);
		}
	}

	bool fdone = false;
	FILETIME start,exit,kernel;
	union
	{
		FILETIME ft;
		unsigned long long int ticks;
	} user;
	PROCESS_MEMORY_COUNTERS meminfo;
	AssignProcessToJobObject(hjob, pinfo.hProcess);
	CTimer timer;
	ResumeThread(pinfo.hThread);
	while (!fdone)
	{
		GetExitCodeProcess(pinfo.hProcess,&exit_code);
		if ( exit_code == STILL_ACTIVE )
		{
			GetProcessTimes(pinfo.hProcess,&start,&exit,&kernel,&user.ft);
			GetProcessMemoryInfo(pinfo.hProcess,&meminfo,sizeof(meminfo));
			if ( prob.TimeLimit != 0 && user.ticks > prob.TimeLimit )
			{
				TerminateJobObject(hjob,1);
				res = rrTimeLimit;
				fdone = true;
			}
			if ( prob.RealTimeLimit != 0 && timer.GetTime() > prob.RealTimeLimit )
			{
				TerminateJobObject(hjob,1);
				res = rrRealTimeLimit;
				fdone = true;
			}
			if ( prob.MemoryLimit != 0 && meminfo.PeakWorkingSetSize > prob.MemoryLimit )
			{
				TerminateJobObject(hjob,1);
				res = rrMemoryLimit;
				fdone = true;
			}
			Sleep(time_atom);
		}
		else
		{
			break;
		}
	}
	IO_COUNTERS count;
	GetProcessTimes(pinfo.hProcess,&start,&exit,&kernel,&user.ft);
	GetProcessMemoryInfo(pinfo.hProcess,&meminfo,sizeof(meminfo));
	GetProcessIoCounters(pinfo.hProcess,&count);
	r_info->TimeWorked = (double)user.ticks / SECS_TO_NSECS;
	r_info->MemoryUsed = meminfo.PeakWorkingSetSize / KBYTES_TO_BYTES;
	r_info->OutputSize = count.WriteTransferCount;
	GetExitCodeProcess(pinfo.hProcess,&exit_code);
	if ( exit_code != 0 )
	{
		crashed = true;
	}
	Clean();
	SetErrorMode(0);
	return res;
#endif
}

int CProcess::GetExitCode()
{
#ifdef WIN32
	return exit_code;
#endif
}

void CProcess::RedirectStdin(std::string std_in)
{
#ifdef WIN32
	SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
	hin = CreateFile(std_in.c_str(),GENERIC_READ,0,&sa,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if ( hin == INVALID_HANDLE_VALUE )
	{
		throw DftestException("Cannot open file to redirect stdandart input", true);
	}
	info.hStdInput = hin;
#endif
}

void CProcess::RedirectStdout(std::string std_out)
{
#ifdef WIN32
	SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
	hout = CreateFile(std_out.c_str(),GENERIC_WRITE,FILE_SHARE_WRITE
		,&sa,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if ( hout == INVALID_HANDLE_VALUE )
	{
		throw DftestException("Cannot open file to redirect stdandart output", true);
	}
	info.hStdOutput = hout;
#endif
}

void CProcess::RedirectStderr(std::string std_err)
{
#ifdef WIN32
	SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
	herr = CreateFile(std_err.c_str(),GENERIC_WRITE,FILE_SHARE_WRITE
		,&sa,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if ( herr == INVALID_HANDLE_VALUE )
	{
		throw DftestException("Cannot open file to redirect stdandart error", true);
	}
	info.hStdError = hout;
#endif
}

std::string GetCrashByCode(int code)
{
#ifdef WIN32
	switch (code)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		return "Access violation";
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		return "Array bounds exceeded";
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		return "Datatype misaligment";
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		return "Too small operand in floating point expression";
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		return "Division by zero";
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		return "Inexact result in floating point expression";
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		return "Invalid floating point operation";
		break;
	case EXCEPTION_FLT_OVERFLOW:
		return "Floating point overflow";
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		return "Stack over/underflow in floating point expression";
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		return "Floating point underflow";
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		return "Illegal processor instruction";
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		return "Integer division by zero";
		break;
	case EXCEPTION_INT_OVERFLOW:
		return "Integer overflow";
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		return "Processor operation is not allowed";
		break;
	case EXCEPTION_STACK_OVERFLOW:
		return "Stack overflow";
		break;
	default:
		return "Unknown error";
		break;
	}
#endif
}

#ifdef WIN32
CTimer::CTimer()
{

	QueryPerformanceCounter((LARGE_INTEGER*)&start);
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
}
#endif

#ifdef WIN32
unsigned long long int CTimer::GetTime()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&stop);
	return (unsigned long long int)(((double)(stop - start) / (double)freq) * SECS_TO_NSECS);
}
#endif
