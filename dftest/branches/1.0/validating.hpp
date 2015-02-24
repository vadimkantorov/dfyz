#ifndef VALIDATING_H
#define VALIDATING_H

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
#include <cmath>

#include "enviroment.hpp"
#include "tinyxml/tinyxml.h"
#include "procutils.hpp"
#include "common.hpp"

const int BUFFER_SIZE = 65536;

enum ValidationStatus
{
	vsOk,
	vsWrongAnswer,
	vsPresentationError,
	vsFailure
};

struct ValidationResult
{
	enum ValidationStatus status;
	std::string comment;
};

class CValidator
{
private:
	FILE *output_file, *answer_file;
	std::string args, cmd, in_string, out_string, ans_string;
	inline void SetFailResult(ValidationResult&,std::string);
	struct ValidationResult BinaryValidate();
	struct ValidationResult TokenValidate();
	struct ValidationResult NumberValidate();
	struct ValidationResult ValidateByXmlOutput(std::string);
	int OpenOutputAndAnswer();
public:
	CValidator(std::string command, std::string arguments, std::string input, std::string output
		,std::string answer);
	~CValidator();
	struct ValidationResult Validate();
};

class CTokenReader
{
private:
	FILE* file;
	unsigned char buffer[BUFFER_SIZE];
	bool has;
	int begin, end;
public:
	CTokenReader(FILE* file);
	~CTokenReader();
	bool HasSymbol();
	unsigned char ReadSymbol();
};

std::string GetToken(FILE*);
bool HasToken(FILE*);
#endif
