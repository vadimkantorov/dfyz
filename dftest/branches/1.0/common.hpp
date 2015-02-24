#ifndef COMMON_H
#define COMMON_H

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

#include "procutils.hpp"
#include "logging.hpp"

const unsigned long long int SECS_TO_NSECS = 10000000;
const unsigned long long int SECS_TO_MSECS = 1000;
const unsigned long long int KBYTES_TO_BYTES = 1024;

const int FAILURE = 4;

struct Validator
{
	std::string Command, Arguments;
};

// TimeLimit is in seconds, MemoryLimit and OutputLimit are in kbytes.
struct Problem
{
	unsigned long long int TimeLimit, RealTimeLimit, MemoryLimit;
	unsigned int OutputLimit;
	unsigned int TestsCount;
	std::string InputFile, OutputFile, TestMask, AnswerMask;
	Validator ProblemValidator;
};

struct ConfigValues
{
	std::string LogFile, RunAsUser, RunAsPassword;
	bool Messages;
	enum LoggingType LogType;
	bool WriteComments;
};

struct SolutionFiles
{
	std::string SolutionSource, SolutionExe;
};

enum TestStatus
{
	tsPassed = 0,
	tsWrongAnswer = 1,
	tsPresentationError = 2,
	tsCpuTimeLimitExceeded = 3,
	tsRealTimeLimitExceeded = 4,
	tsMemoryLimitExceeded = 5,
	tsSecurityViolation = 6,
	tsCrash = 7,
	tsOutputLimitExceeded = 8
};

struct TestResult
{
	enum TestStatus status;
	std::string info;
	unsigned long long int memUsed, outputSize;
	double timeWorked;
};

std::string inline GetLastSystemError()
{
#ifdef WIN32
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), 0, (LPTSTR) &lpMsgBuf, 0, NULL);
	CharToOem((const char*)lpMsgBuf,(char*)lpMsgBuf);
	std::string ret((char*)lpMsgBuf);
	return ret;
	LocalFree(lpMsgBuf);
#endif
}

class DftestException
{
private:
	std::string message;
public:
	std::string FailureMessage() const
	{
		return message;
	}
	DftestException(std::string message, bool append_sys_msg)
	{
		this->message = message;
		if ( append_sys_msg )
		{
			this->message += ": ";
			this->message += GetLastSystemError();
		}
	}
};

void inline LowercaseString(std::string& str)
{
	for ( unsigned int i = 0; i < str.size(); i++ )
	{
		str[i] = tolower(str[i]);
	}
}

#endif
