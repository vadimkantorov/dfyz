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
 
#include "testing.hpp"

CTester::CTester()
{
	process_name = PathCombine(runDir,files.SolutionExe);
	sproc = new CProcess(process_name,"",runDir);
}

CTester::~CTester()
{
	delete sproc;
}

bool CTester::TestExists(unsigned int num)
{
	std::string test = PathCombine(problemDir,prob.TestMask);
	std::string::size_type sharps = test.find("##");
	if ( sharps == test.npos )
	{
		throw DftestException("Test mask doesn't contain '##' symbols", false);
	}
	char s[11];
	sprintf(s,"%02u",num);
	test.replace(sharps,2,s);
	return PathExists(test)?true:false;
}

TestResult CTester::PerformTest(unsigned int test_number)
{
	TestResult result;
	ValidationResult val_res;
	bool use_stdin = false, use_stdout = false;
	std::string in, out;
	// Form various full filenames
	std::string test = PathCombine(problemDir,prob.TestMask);
	std::string ans = PathCombine(problemDir,prob.AnswerMask);
	if ( prob.InputFile == "#stdin")
	{
		use_stdin = true;
		in = PathCombine(runDir,"$std.in$");
	}
	else
	{
		in = PathCombine(runDir,prob.InputFile);
	}
	if ( prob.OutputFile == "#stdout" )
	{
		use_stdout = true;
		out = PathCombine(runDir,"$std.out$");
	}
	else
	{
		out = PathCombine(runDir,prob.OutputFile);
	}
	if ( in == out )
	{
		throw DftestException("Input and output files must be different", false);
	}
	// Replace '##' or '###' symbols in test and answer masks with test number
	char num[12];
	sprintf(num,"%02u",test_number);
	std::string::size_type sharps = test.find("##");
	if ( sharps == test.npos )
	{
		throw DftestException("Test mask doesn't contain '##' symbols", false);
	}
	else
	{
		test.replace(sharps,2,num);
	}
	sharps = ans.find("##");
	if ( sharps == ans.npos )
	{
		throw DftestException("Answer mask doesn't contain '##' symbols", false);
	}
	else
	{
		ans.replace(sharps,2,num);
	}
	if ( !FileCopy(test,in) )
	{
		throw DftestException("Cannot copy test file", true);
	}
	CFileHandler handler;
	handler.PushFile(in);
	handler.PushFile(out);
	if ( use_stdin )
	{
		sproc->RedirectStdin(in);
	}
	else
	{
#ifdef WIN32
		sproc->RedirectStdin("nul");
#else
		sproc->RedirectStdin("/dev/null");
#endif
	}
	if ( use_stdout )
	{
		sproc->RedirectStdout(out);
	}
	else
	{
#ifdef WIN32
		sproc->RedirectStdout("nul");
#else
		sproc->RedirectStdout("/dev/null");
#endif
	}
	if ( values.RunAsUser != "" )
	{
		sproc->RunAs(values.RunAsUser,values.RunAsPassword);
	}
	bool ret = false;
	RunResult rres;
	RunInfo rinfo;
	rres = sproc->StartWithAllLimits(&rinfo);
	result.timeWorked = rinfo.TimeWorked;
	result.memUsed = rinfo.MemoryUsed;
	result.outputSize = rinfo.OutputSize;
	if ( rres == rrTimeLimit )
	{
		result.status = tsCpuTimeLimitExceeded;
		return result;
	}
	else if ( rres == rrRealTimeLimit )
	{
		result.status = tsRealTimeLimitExceeded;
		return result;
	}
	else if ( rres == rrMemoryLimit )
	{
		result.status = tsMemoryLimitExceeded;
		return result;
	}
	if ( prob.OutputLimit != 0 && (FileSize(out) > prob.OutputLimit) )
	{
		result.status = tsOutputLimitExceeded;
		return result;
	}
	if ( sproc->Crashed() )
	{
		result.status = tsCrash;
		result.info = GetCrashByCode(sproc->GetExitCode());
		return result;
	}
	else
	{
		// Create validator in else statement to ensure that destructor will be called before file deleting
		CValidator val(prob.ProblemValidator.Command,prob.ProblemValidator.Arguments,test,out,ans);
		val_res = val.Validate();
	}
	switch ( val_res.status )
	{
	case vsOk:
		result.status = tsPassed;
		break;
	case vsWrongAnswer:
		result.status = tsWrongAnswer;
		break;
	case vsPresentationError:
		result.status = tsPresentationError;
		break;
	case vsFailure:
		throw DftestException(val_res.comment, false);
		break;
	}
	result.info = val_res.comment;
	return result;
}
