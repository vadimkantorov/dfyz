#ifndef LOGGING_H
#define LOGGING_H

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
 
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>

// Represents log type to write information about testing
enum LoggingType
{
	ltConsole,
	ltXsl,
	ltXml
};

#ifdef WIN32
#define COLOR_DEFAULT 0x0007
#define COLOR_RED     0x0004
#define COLOR_GREEN   0x0002
#define COLOR_YELLOW  0x0006
#define COLOR_BLUE    0x0001
#define COLOR_MAGENTA 0x0005
#define COLOR_CYAN    0x0003
#define COLOR_WHITE   0x0008
#else
#define COLOR_DEFAULT "\033[1;0m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_MAGENTA "\033[1;35m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_WHITE   "\033[1;37m"
#endif

#include "enviroment.hpp"
#include "xmlwriter.hpp"
#include "common.hpp"
#include "fileutils.hpp"

// Xsl file content
const std::string xsl_document = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\
<xsl:stylesheet version=\"1.0\"\
xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">\
<xsl:template match=\"/tests\">\
<html><body>\
<style type=\"text/css\">\
table.main {\
border-collapse: collapse;\
}\
h {\
text-align: left;\
}\
td, table, th {\
border: 1pt solid gray;\
padding: 1pt 10pt 1pt 10pt;\
}\
</style>\
<table class=\"main\">\
<tr>\
<th>\
<xsl:text>Test number</xsl:text>\
</th>\
<th>\
<xsl:text>Result</xsl:text>\
</th>\
<th>\
<xsl:text>Checker comment</xsl:text>\
</th>\
<th>\
<xsl:text>Time worked</xsl:text>\
</th>\
<th>\
<xsl:text>Memory used</xsl:text>\
</th>\
<th>\
<xsl:text>Output size</xsl:text>\
</th>\
<xsl:apply-templates select=\"test\"/>\
</tr>\
</table>\
</body></html>\
</xsl:template>\
<xsl:template match=\"test\">\
<tr bgcolor=\"#e0ffe0\">\
<td>\
<xsl:value-of select=\"@number\"/>	\
</td>\
<td>\
<xsl:value-of select=\"@result\"/>\
</td>\
<td>\
<xsl:value-of select=\".\"/>\
</td>\
<td>\
<xsl:value-of select=\"@time\"/>\
<xsl:text> seconds</xsl:text>\
</td>\
<td>\
<xsl:value-of select=\"@memory\"/>\
<xsl:text> KB</xsl:text>\
</td>\
<td>\
<xsl:value-of select=\"@output\"/>\
<xsl:text> bytes</xsl:text>\
</td>\
</tr>\
</xsl:template>\
</xsl:stylesheet>";

// Represents color setting for ltHtml and ltConsole log formats
enum LogColor
{
	lcRed,
	lcGreen,
	lcYellow,
	lcBlue,
	lcMagenta,
	lcCyan,
	lcWhite
};

class CLogger
{
private:
	std::string log_name, fail_name, style_name;
	std::string detail_statuses[9];
	std::string short_statuses[9];
	bool write_comment, fail_msgs;
	class CXmlWriter* xml;
	enum LoggingType log_type;
public:
	CLogger();
	~CLogger();
	void Accepted();
	void WriteTestBegin(unsigned int number);
	void WriteTestEnd(struct TestResult result);
	void Failure(class DftestException& exception);
};

void SetColor(enum LogColor color);
void ResetColor();

#endif
