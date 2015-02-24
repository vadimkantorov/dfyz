#ifndef COMPILING_H
#define COMPILING_H

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

#include <map>

#include "common.hpp"
#include "logging.hpp"
#include "procutils.hpp"
#include "fileutils.hpp"
#include "tinyxml/tinyxml.h"

enum NodeType
{
	DOCUMENT,
	ELEMENT,
	COMMENT,
	UNKNOWN,
	TEXT,
	DECLARATION,
	TYPECOUNT
};

class CCompiler
{
public:
	CCompiler();
	~CCompiler();
	bool Compile(std::string source, std::string exe, std::string report);
};

#endif