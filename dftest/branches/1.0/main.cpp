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
#endif

#include "enviroment.hpp"
#include "logging.hpp"
#include "fileutils.hpp"
#include "validating.hpp"
#include "procutils.hpp"
#include "compiling.hpp"
#include "testing.hpp"
#include "config.hpp"

const std::string usage_message = "Usage: dftest.exe <problem-dir> <run-dir> [-source source]\n\t\
[-exe executable] [-report report]\nOptions:\n\tproblem-dir\tDirectory \
containing problem.xml, tests and checker\n\trun-dir\t\tDirectory \
which contains solution source, solution\n\t\t\texecutable and which is used \
as working directory\n\t\t\twhile testing\n\t-source\tSpecify a relative path \
to solution source from run-dir\n\t-exe\t\tSpecify a relative path to solution \
executable from\n\t\t\trun-dir\n\t-report\tSpecify a relative path from run-dir to \
file\n\t\t\tin which compiler output will be stored\n\t-help\t\tShow help\n\nYou should specify at \
least one of -exe and -source switches.\nIf both are present, solution will \
be compiled from -source to -exe,\nand then tested.\nIf only -source is present\
, it will be compiled from -source to\n\"solution.exe\" without testing.\n\
If there is only -exe switch then -exe will be tested without compilation.\n\n\
Use dftest.xml to setup DfTest behaviour";

const std::string PROBLEM_XML = "problem.xml";
const std::string CONFIG_XML = "dftest.xml";
const int INVALID_CMDLINE = 2, COMPILATION_ERROR = 1;

std::string runDir, problemDir, report;
struct Problem prob;
struct ConfigValues values;
struct SolutionFiles files;
Compilers comp_map;
static char* serv_name = "DfTest Service";

bool HelpSwitch(int argc, char* argv[])
{
	for ( int i = 1; i < argc; i++ )
	{
		if ( strcmpi(argv[i],"-help") == 0 )
		{
			return true;
		}
	}
	return false;
}

bool ProcessCommandLine(int argc, char* argv[])
{
	int mandatory_count = 0;
	for ( int i = 1; i < argc; i++ )
	{
		std::string param = argv[i];
		LowercaseString(param);
		if ( param == "-source" )
		{
			if ( i+1 < argc )
			{
				files.SolutionSource = argv[i+1];
				i++;
			}
		}
		else if ( param == "-exe" )
		{
			if ( i+1 < argc )
			{
				files.SolutionExe = argv[i+1];
				i++;
			}
		}
		else if ( param == "-report" )
		{
			if ( i+1 < argc )
			{
				report = argv[i+1];
				i++;
			}
		}
		else
		{
			switch (mandatory_count)
			{
			case 0:
				problemDir = argv[i];
				mandatory_count++;
				break;
			case 1:
				runDir = argv[i];
				mandatory_count++;
				break;
			}
		}
	}
	return (mandatory_count >= 1 && (files.SolutionExe != "" || files.SolutionSource != "") ) 
		? true : false;
}

/* Exit code can be one of the following:
 * 0 - testing suceeded (parse log for details)
 * any other - testing failed due to unexpected error (reason can be found in failure file)
 */
int main(int argc, char* argv[])
{
	if ( HelpSwitch(argc, argv) )
	{
		std::cout << usage_message;
		exit(EXIT_SUCCESS);
	}
	if ( argc < 3 || !ProcessCommandLine(argc,argv) )
	{
		std::cout << usage_message;
		exit(INVALID_CMDLINE);
	}
	CLogger *logger = NULL;
	try
	{
		CConfigParser parser(PathCombine(problemDir,PROBLEM_XML), "dftest.xml");
		values = parser.ParseIni();
		logger = new CLogger();
		if ( !PathExists(problemDir) )
		{
			throw DftestException("Problem directory doesn't exist", false);
		}
		if ( !PathExists(runDir) )
		{
			throw DftestException ("Run directory doesn't exist", false);
		}
		if ( files.SolutionSource != "" )
		{
			comp_map = parser.ParseCompilers();
			CCompiler comp;
			if ( !comp.Compile(PathCombine(runDir, files.SolutionSource), 
				PathCombine(runDir, (files.SolutionExe != "") ? files.SolutionExe : "solution.exe"), 
				(report != "") ? PathCombine(runDir, report) : "") )
			{
				if ( values.Messages )
				{
					SetColor(lcYellow);
					std::cout << "Compilation error: see compiler output for details" << std::endl;
					ResetColor();
				}
				exit(COMPILATION_ERROR);
			}
			else
			{
				if ( values.Messages )
				{
					SetColor(lcGreen);
					std::cout << "Compilation succeeded" << std::endl;
					ResetColor();
				}
			}
		}
		if ( files.SolutionExe != "" )
		{
			prob = parser.ParseProblem();
			CTester test;
			if ( prob.TestsCount == 0 )
			{
				for ( unsigned int i = 1; test.TestExists(i); i++ );
				if ( i == 1 )
				{
					throw DftestException("No tests were found", false);
				}
				prob.TestsCount = i-1;
			}
			TestResult res;
			TestStatus status;
			for ( unsigned int i = 1; i <= prob.TestsCount; i++ )
			{
				logger->WriteTestBegin(i);
				res = test.PerformTest(i);
				logger->WriteTestEnd(res);
				status = res.status;
				if ( res.status != tsPassed )
				{
					break;
				}
			}
			if ( res.status == tsPassed )
			{
				logger->Accepted();
			}
		}
	}
	catch (DftestException& e)
	{
		if ( logger )
		{
			logger->Failure(e);
			delete logger;
		}
		exit ( FAILURE );
	}
	if ( logger )
	{
		delete logger;
	}
	return 0;
}

