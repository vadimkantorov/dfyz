#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

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
 
#include "common.hpp"
#include "enviroment.hpp"
#include "fileutils.hpp"
#include "tinyxml/tinyxml.h"

class CConfigParser
{
private:
	std::string problem_file,ini_file;
	TiXmlDocument *problem_doc, *ini_doc;
public:
	CConfigParser(std::string problem, std::string ini);
	~CConfigParser();
	Problem ParseProblem();
	ConfigValues ParseIni();
	Compilers ParseCompilers();
};

#endif