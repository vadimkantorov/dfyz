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
 
#include "config.hpp"

std::string inline GetAttributeOrDie(const TiXmlElement* elem, std::string name)
{
	const char* attr = elem->Attribute(name);
	if ( !attr )
	{
		throw DftestException("Cannot find \"" + name + "\" attribute", false);
	}
	else
	{
		return attr;
	}
}

void inline SetAttributeIfExists(const TiXmlElement* elem, std::string name, std::string& to)
{
	const char* attr = elem->Attribute(name);
	if ( attr )
	{
		to = attr;
	}
}

CConfigParser::CConfigParser(std::string problem,std::string ini)
{
	problem_file = problem;
	ini_file = ini;
	problem_doc = new TiXmlDocument();
	ini_doc = new TiXmlDocument(ini);
	if ( ini_doc->Error() )
	{
		std::string msg = "Cannot load main configuration file: ";
		msg += ini_doc->ErrorDesc();
		throw DftestException(msg, false);
	}
	else
	{
		ini_doc->LoadFile();
	}
}

CConfigParser::~CConfigParser()
{
	delete problem_doc;
	delete ini_doc;
}

Problem CConfigParser::ParseProblem()
{
	Problem prb;
	prb.RealTimeLimit = 0;
	prb.OutputLimit = 0;
	prb.TimeLimit = 0;
	prb.MemoryLimit = 0;
	prb.ProblemValidator.Command = "#binary";
	prb.ProblemValidator.Arguments = "{input} {output} {answer} [result.xml] -appes";

	if ( !problem_doc->LoadFile(problem_file) )
	{
		std::string msg = problem_doc->ErrorDesc();
		msg += " while parsing ";
		msg += GetBasename(problem_file);
		throw DftestException(msg, false);
	}
	TiXmlElement* root = problem_doc->FirstChildElement("problem");
	if ( !root )
	{
		throw DftestException("Root element must be called \"problems\" in " + GetBasename(problem_file), false);
	}
	TiXmlElement* tests = root->FirstChildElement("tests");
	if ( !tests )
	{
		throw DftestException("Cannot find \"tests\" element in " + GetBasename(problem_file), false);
	}
	std::string tests_count = GetAttributeOrDie(tests,"count");
	LowercaseString(tests_count);
	if ( tests_count == "auto" )
	{
		prb.TestsCount = 0;
	}
	else
	{
		prb.TestsCount = atoi(tests_count.c_str());
	}
	prb.AnswerMask = GetAttributeOrDie(tests,"answers");
	prb.TestMask = GetAttributeOrDie(tests,"tests");
	prb.InputFile = GetAttributeOrDie(tests,"input");
	prb.OutputFile = GetAttributeOrDie(tests,"output");
	
	TiXmlElement* limits = tests->FirstChildElement("limits");
	if ( limits )
	{
		double tl = 0.0, rtl = 0.0;
		limits->Attribute("cpu-time",&tl);
		prb.TimeLimit = tl * SECS_TO_NSECS;
		if ( (prb.TimeLimit < 0) || (prb.TimeLimit > 0 && prb.TimeLimit < 10000) )
		{
			prb.TimeLimit = 0;
		}
		limits->Attribute("real-time",&rtl);
		prb.RealTimeLimit = rtl * SECS_TO_NSECS;
		if ( (prb.RealTimeLimit < 0) || (prb.RealTimeLimit > 0 && prb.RealTimeLimit < 10000) )
		{
			prb.RealTimeLimit = 0;
		}
		const char* ml = limits->Attribute("memory");
		if ( ml )
		{
			int len = strlen(ml);
			long long int m = 0L;
			unsigned long long int coef = 1;
			if ( len > 0 )
			{
				if ( tolower(ml[len-1]) == 'm' )
				{
					coef = 1024;
					int len = strlen(ml);
					char mml[20];
					memset(mml,0,sizeof(mml));
					strncpy(mml,ml,sizeof(mml));
					mml[len-1] = '\0';
					m = atol(mml);
					if ( m > 0 )
					{
						prb.MemoryLimit = m * 1048576;
					}
				}
				else
				{
					m = atol(ml);
					prb.MemoryLimit = m * 1024;
				}
			}
		}
		int ol = 0;
		limits->Attribute("output",&ol);
		if ( ol > 0 )
		{
			prb.OutputLimit = ol;
		}
	}
	TiXmlElement* validator = tests->FirstChildElement("validator");
	if ( validator )
	{
		SetAttributeIfExists(validator,"command",prb.ProblemValidator.Command);
		SetAttributeIfExists(validator,"arguments",prb.ProblemValidator.Arguments);
	}
	return prb;
}

ConfigValues CConfigParser::ParseIni()
{
	ConfigValues ini;
	ini.LogType = ltConsole;
	ini.WriteComments = true;
	ini.Messages = true;
	TiXmlElement* root = ini_doc->FirstChildElement("config");
	if ( root )
	{
		TiXmlElement* log = root->FirstChildElement("log");
		if ( log )
		{
			std::string failures;
			SetAttributeIfExists(log,"messages",failures);
			LowercaseString(failures);
			if ( failures == "false" )
			{
				ini.Messages = false;
			}
			SetAttributeIfExists(log,"file",ini.LogFile);
			std::string type = "console";
			SetAttributeIfExists(log,"type",type);
			LowercaseString(type);
			if ( type == "xsl" )
			{
				ini.LogType = ltXsl;
			}
			else if ( type == "xml" )
			{
				ini.LogType = ltXml;
			}
			else
			{
				ini.LogType = ltConsole;
			}
			std::string comms;
			SetAttributeIfExists(log,"comments",comms);
			LowercaseString(comms);
			if ( comms == "false" )
			{
				ini.WriteComments = false;
			}
		}
		TiXmlElement* runas = root->FirstChildElement("runas");
		if ( runas )
		{
			const char* user = runas->Attribute("user");
			if ( user && (strcmp(user,"") != 0) )
			{
				ini.RunAsUser = user;
				const char* passwd = runas->Attribute("password");
				if ( passwd )
				{
					ini.RunAsPassword = passwd;
				}
			}
		}
	}
	return ini;
}

Compilers CConfigParser::ParseCompilers()
{
	Compilers map;
	if ( !ini_doc->Error() )
	{
		TiXmlElement* root = ini_doc->FirstChildElement("config");
		if ( root ) 
		{
			TiXmlElement* compilers = root->FirstChildElement("compilers");
			if ( compilers )
			{
				TiXmlElement* compiler = compilers->FirstChildElement("compiler");
				while ( compiler )
				{
					const char* extension = compiler->Attribute("extension");
					const char* command = compiler->Attribute("command");
					const char* arguments = compiler->Attribute("arguments");
					if ( !extension || (strcmp(extension,"") == 0) ||
						!command || (strcmp(command,"") == 0) ||
						!arguments || (strcmp(arguments,"") == 0) )
					{
						throw DftestException("\"compiler\" tag doesn't have valid attributes", false);
					}
					CompilerInfo info;
					info.Executable = command;
					info.Arguments = arguments;
					map[extension] = info;
					compiler = compiler->NextSiblingElement("compiler");
				}
			}
		}
	}
	return map;
}