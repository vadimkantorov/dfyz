#ifndef PROCUTILS_H
#define PROCUTILS_H

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

#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <psapi.h>
#endif

#include "common.hpp"
#include <string>

const unsigned int time_atom = 20;

typedef BOOL (WINAPI *LogonFunction) (LPCWSTR,LPCWSTR,LPCWSTR,DWORD,LPCWSTR,LPWSTR,DWORD,
									  LPVOID,LPCWSTR,LPSTARTUPINFOW,LPPROCESS_INFORMATION);
enum RunResult
{
	rrSuccess,
	rrTimeLimit,
	rrRealTimeLimit,
	rrMemoryLimit,
	rrSecurityViolation,
};

struct RunInfo
{
	double TimeWorked;
	unsigned long long int MemoryUsed, OutputSize;
};

std::string GetCrashByCode(int code);

class CProcess
{
private:
	std::string file, args, work_dir, username, password;
	bool crashed;
#ifdef WIN32
	DWORD exit_code;
	PROCESS_INFORMATION pinfo;
	STARTUPINFO info;
	HANDLE hin, hout, herr, hjob, hport;
	void Clean();
#endif
public:
	CProcess(std::string filename,std::string arguments,std::string working_directory);
	~CProcess();
	int GetExitCode();
	bool Crashed();
	void RedirectStdin(std::string std_in);
	void RedirectStdout(std::string std_out);
	void RedirectStderr(std::string std_err);
	void RunAs(std::string usr,std::string passwd);
	bool StartWithTL(unsigned int time_limit);
	RunResult StartWithAllLimits(RunInfo* r_info);
};

#ifdef WIN32
class CTimer
{
private:
	unsigned long long int freq, start, stop;
public:
	CTimer();
	unsigned long long int GetTime();
};
#endif

#endif
