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
 
#include "logging.hpp"

CLogger::CLogger()
{
	log_type = values.LogType;
	log_name = PathCombine(runDir,values.LogFile);
	write_comment = values.WriteComments;
	xml = NULL;
	fail_msgs = values.Messages;
	
	if ( log_type == ltXml || log_type == ltXsl )
	{
		try
		{
			xml = new CXmlWriter(log_name);
		}
		catch (DftestException& e)
		{
			Failure(e);
		}
		xml->XmlWriteVersion();
		if ( log_type == ltXsl )
		{
			std::string style_file = "style.xsl";
			style_name = PathCombine(runDir,style_file);
			xml->XmlWriteStylesheet(style_file);
			std::ofstream style(style_name.c_str(),std::ios_base::out);
			if ( !style )
			{
				Failure(DftestException("Cannot open xsl stylesheet for writing", true));
			}
			style << xsl_document;
		}
		xml->XmlOpenTag("tests");
	}

	detail_statuses[0] = "Passed";
	detail_statuses[1] = "Wrong answer";
	detail_statuses[2] = "Presenation error";
	detail_statuses[3] = "CPU time limit exceeded";
	detail_statuses[4] = "Real time limit exceeded";
	detail_statuses[5] = "Memory limit exceeded";
	detail_statuses[6] = "Security violation";
	detail_statuses[7] = "Runtime error";
	detail_statuses[8] = "Output limit exceeded";

	short_statuses[0] = "PASSED";
	short_statuses[1] = "WA";
	short_statuses[2] = "PE";
	short_statuses[3] = "CTLE";
	short_statuses[4] = "RTLE";
	short_statuses[5] = "MLE";
	short_statuses[6] = "SV";
	short_statuses[7] = "RE";
	short_statuses[8] = "OLE";
}

void CLogger::Failure(DftestException& exception)
{
	if ( fail_msgs )
	{
		SetColor(lcRed);
		std::cout << "ERROR ";
		ResetColor();
		std::cout << exception.FailureMessage();
	}
	if ( log_type != ltConsole )
	{
		if ( log_type == ltXml || log_type == ltXsl )
		{
			delete xml;
			xml = NULL;
			if ( log_type == ltXsl )
			{
				FileDelete(style_name);
			}
		}
		FileDelete(log_name);
	}
	exit(FAILURE);
}

void CLogger::WriteTestBegin(unsigned int number)
{
	if ( log_type == ltConsole )
	{
		std::cout << "Test " << std::setfill('0') << std::setw(2) << number << ": ";
	}
	else if ( log_type == ltXml || log_type == ltXsl )
	{
		xml->XmlOpenTag("test");
		char s[5];
		sprintf(s,"%d",number);
		xml->XmlWriteAttribute("number",s);
	}
}

void CLogger::WriteTestEnd(TestResult result)
{
	if ( log_type == ltConsole )
	{
		ResetColor();
		std::cout.setf(std::ios_base::fixed);
		std::cout << std::setprecision(3)<< result.timeWorked << " sec, " 
			<< result.memUsed << " KB, " << result.outputSize << " bytes ";
		switch (result.status)
		{
			case tsPassed:
				SetColor(lcGreen);
				break;
			case tsWrongAnswer:
				SetColor(lcRed);
				break;
			case tsPresentationError:
				SetColor(lcRed);
				break;
			case tsCpuTimeLimitExceeded:
				SetColor(lcCyan);
				break;
			case tsRealTimeLimitExceeded:
				SetColor(lcBlue);
				break;
			case tsMemoryLimitExceeded:
				SetColor(lcMagenta);
				break;
			case tsCrash:
				SetColor(lcYellow);
				break;
			case tsSecurityViolation:
				SetColor(lcWhite);
				break;
		}
		std::cout << detail_statuses[result.status];
		ResetColor();
		if ( write_comment )
		{
			std::cout << " " << result.info;
		}
		std::cout << std::endl;
	}
	else if ( log_type == ltXml || log_type == ltXsl )
	{
		char s[15];
		sprintf(s,"%.3lf",result.timeWorked);
		xml->XmlWriteAttribute("time",s);
		sprintf(s,"%d",result.memUsed);
		xml->XmlWriteAttribute("memory",s);
		sprintf(s,"%d",result.outputSize);
		xml->XmlWriteAttribute("output",s);
		xml->XmlWriteAttribute("result",short_statuses[result.status]);
		if ( write_comment )
		{
			xml->XmlWriteBody(result.info);
		}
		xml->XmlCloseTag();
	}
}

void CLogger::Accepted()
{
	if ( log_type == ltConsole )
	{
		SetColor(lcGreen);
		std::cout << "Accepted";
		ResetColor();
	}
}

CLogger::~CLogger()
{
	if ( xml != NULL )
	{
		delete xml;
	}
}

void SetColor(enum LogColor color)
{
#ifdef WIN32
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	switch ( color )
	{
		case lcYellow:
			SetConsoleTextAttribute(h,COLOR_YELLOW|COLOR_WHITE);
			break;			
		case lcRed:
			SetConsoleTextAttribute(h,COLOR_RED|COLOR_WHITE);
			break;
		case lcGreen:
			SetConsoleTextAttribute(h,COLOR_GREEN|COLOR_WHITE);
			break;
		case lcCyan:
			SetConsoleTextAttribute(h,COLOR_CYAN|COLOR_WHITE);
			break;
		case lcBlue:
			SetConsoleTextAttribute(h,COLOR_BLUE|COLOR_WHITE);
			break;
		case lcMagenta:
		SetConsoleTextAttribute(h,COLOR_MAGENTA|COLOR_WHITE);
		break;
		case lcWhite:
			SetConsoleTextAttribute(h,COLOR_DEFAULT|COLOR_WHITE);
			break;
	}
#else
	switch ( color )
	{
		case lcYellow:
			std::cout << COLOR_YELLOW;
			break;
		case lcRed:
			std::cout << COLOR_RED;
			break;
		case lcGreen:
			std::cout << COLOR_GREEN;
			break;
		case lcCyan:
			std::cout << COLOR_CYAN;
			break;
		case lcBlue:
			std::cout << COLOR_BLUE;
			break;
		case lcMagenta:
			std::cout << COLOR_MAGENTA;
			break;
		case lcWhite:
			std::cout << COLOR_WHITE;
			break;
	}
#endif
}

void ResetColor()
{
#ifdef WIN32
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, COLOR_DEFAULT);
#else	
    printf(COLOR_DEFAULT);
#endif
}