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

#include "compiling.hpp"

CCompiler::CCompiler()
{}

CCompiler::~CCompiler()
{}

bool CCompiler::Compile(std::string source, std::string exe, std::string report)
{
	if ( !PathExists(source) )
	{
		throw DftestException("Source file doesn't exist", false);
	}
	std::string suffix = GetSuffix(source);
	if ( comp_map.find(suffix) == comp_map.end() )
	{
		throw DftestException("Don't know how to compile ." + suffix + " files", false);
	}
	else
	{
		CompilerInfo inf = comp_map[suffix];
		std::string cmdline = inf.Arguments;
		std::string::size_type src = cmdline.find("{S}");
		if ( src == cmdline.npos )
		{
			throw DftestException("Compiler arguments should contain {S} substring", false);
		}
		else
		{
			cmdline.replace(src,3,source);
		}
		std::string::size_type dst = cmdline.find("{T}");
		if ( dst == cmdline.npos )
		{
			throw DftestException("Compiler arguments should contain {T} substrings", false);
		}
		else
		{
			cmdline.replace(dst,3,exe);
		}
		CProcess comp_proc(inf.Executable,cmdline,GetDirname(source));
		if ( report != "" )
		{
			comp_proc.RedirectStdout(report);
			comp_proc.RedirectStderr(report);
		}
		if ( !comp_proc.StartWithTL(60) )
		{
			throw DftestException("Time limit exceeded for compilation", false);
		}
		else
		{
			return ( comp_proc.GetExitCode() == 0 )?true:false;
		}
	}
}
