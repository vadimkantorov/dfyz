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
 
#include "validating.hpp"

CValidator::CValidator(std::string validator_command, std::string arguments, 
					   std::string input, std::string output, std::string answer)
{
	in_string = input;
	out_string = output;
	ans_string = answer;
	cmd = validator_command;
	args = arguments;
	output_file = answer_file = NULL;
}

CValidator::~CValidator()
{
	if ( output_file != NULL )
	{
		fclose(output_file);
	}
	if ( answer_file != NULL )
	{
		fclose(answer_file);
	}
}

inline void CValidator::SetFailResult(ValidationResult& res, std::string comment)
{
	res.status = vsFailure;
	res.comment = comment;
}

struct ValidationResult CValidator::Validate()
{
	ValidationResult res;
	bool builtin = false;
	LowercaseString(cmd);
	std::string result_file;

	if ( cmd[0] == '#' )
	{
		builtin = true;
		cmd = cmd.substr(1);
	}

	if ( builtin )
	{
		if ( cmd == "binary" )
		{
			return BinaryValidate();
		}
		else if ( cmd == "token" )
		{
			return TokenValidate();
		}
		else if ( cmd == "number" )
		{
			return NumberValidate();
		}
		else
		{
			SetFailResult(res,"Unknown builtin validator");
			return res;
		}
	}
	else
	{
		cmd = PathCombine(problemDir,cmd);
		// Parse {input|output|answer} ans [result] directives
		in_string = "\"" + in_string + "\"";
		out_string = "\"" + out_string + "\"";
		ans_string = "\"" + ans_string + "\"";
		std::string::size_type pos = 0;
		std::string istr = "{input}", ostr = "{output}", astr = "{answer}";
		pos = args.find(istr);
		if ( pos != args.npos )
		{
			args.replace(pos,istr.size(),in_string);
		}
		pos = args.find(ostr);
		if ( pos != args.npos )
		{
			args.replace(pos,ostr.size(),out_string);
		}
		pos = args.find(astr);
		if ( pos != args.npos )
		{
			args.replace(pos,astr.size(),ans_string);
		}
		std::string::size_type open_paren = args.find("[");
		std::string::size_type close_paren = args.find("]");
		if ( open_paren == args.npos || close_paren == args.npos || open_paren > close_paren )
		{
			SetFailResult(res,"Invalid validator arguments");
			return res;
		}
		else
		{
			result_file = PathCombine(runDir,args.substr(open_paren+1,close_paren-open_paren-1));
			args.replace(open_paren,close_paren-open_paren+1,result_file);
		}
		bool ret;
		CProcess vproc(cmd,args,runDir);
		ret = vproc.StartWithTL(10);
		if ( !ret )
		{
			SetFailResult(res,"Time limit exceeded for validation program");
			return res;
		}
		if ( vproc.GetExitCode() != 0 )
		{
			SetFailResult(res,"Validator process exited with non-zero exit code");
		}
		return ValidateByXmlOutput(result_file);
	}
}

int CValidator::OpenOutputAndAnswer()
{
	output_file = fopen(out_string.c_str(),"rb");
	answer_file = fopen(ans_string.c_str(),"rb");

	if ( !output_file )
	{
		return -1;
	}
	else if ( !answer_file )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

struct ValidationResult CValidator::BinaryValidate()
{
	bool ignore_case = false;
	ValidationResult res;
	int out_read = 0, ans_read = 0;
	unsigned char out_buffer[BUFFER_SIZE], ans_buffer[BUFFER_SIZE];

	switch ( OpenOutputAndAnswer() )
	{
	case -1:
		res.status = vsPresentationError;
		res.comment = "Cannot open solution's output";
		return res;
		break;
	case 1:
		SetFailResult(res,"Cannot open answer file");
		return res;
		break;
	}

	while ( !feof(answer_file) )
	{
		memset(out_buffer,0,BUFFER_SIZE);
		memset(ans_buffer,0,BUFFER_SIZE);
		out_read = (int)fread(out_buffer,sizeof(unsigned char),BUFFER_SIZE,output_file);
		ans_read = (int)fread(ans_buffer,sizeof(unsigned char),BUFFER_SIZE,answer_file);
		if ( out_read != ans_read )
		{
			res.status = vsWrongAnswer;
			res.comment = "Different file sizes";
			return res;
		}
		if ( memcmp(out_buffer,ans_buffer,ans_read) != 0 )
		{
			res.status = vsWrongAnswer;
			res.comment = "Different file contents";
			return res;
		}
	}
	res.status = vsOk;
	res.comment = "Binary check suceeded";
	return res;
}

struct ValidationResult CValidator::TokenValidate()
{
	ValidationResult res;
	switch ( OpenOutputAndAnswer() )
	{
	case -1:
		res.status = vsPresentationError;
		res.comment = "Cannot open solution's output";
		return res;
		break;
	case 1:
		SetFailResult(res,"Cannot open answer file");
		return res;
		break;
	}

	CTokenReader ans_rdr(answer_file);
	CTokenReader out_rdr(output_file);
	while ( ans_rdr.HasSymbol() )
	{
		if ( !out_rdr.HasSymbol() )
		{
			res.status = vsWrongAnswer;
			res.comment = "Too few symbols in solution's output";
			return res;
		}
		unsigned char a = ans_rdr.ReadSymbol();
		while ( isspace(a) )
		{
			a = ans_rdr.ReadSymbol();
		}
		unsigned char o = out_rdr.ReadSymbol();
		while ( isspace(o) )
		{
			o = out_rdr.ReadSymbol();
		}
		if ( o != a )
		{
			res.status = vsWrongAnswer;
			res.comment = "Wrong symbol in solution's output: ";
			res.comment += o;
			res.comment += " != ";
			res.comment += a;
			return res;
		}
	} 
	res.status = vsOk;
	res.comment = "Token check suceeded";
	return res;
}

struct ValidationResult CValidator::NumberValidate()
{
	ValidationResult res;
	double out_val = 0, ans_val = 0;
	const double precision = 1e-10;

	switch ( OpenOutputAndAnswer() )
	{
	case -1:
		res.status = vsPresentationError;
		res.comment = "Cannot open solution's output";
		return res;
		break;
	case 1:
		SetFailResult(res,"Cannot open answer file");
		return res;
		break;
	}

	while ( fscanf(answer_file,"%lf",&ans_val) == 1)
	{
		if ( fscanf(output_file,"%lf",&out_val) != 1 )
		{
			res.status = vsPresentationError;
			res.comment = "Cannot read number from solution's output";
			return res;
		}
		else
		{
			if ( fabs(ans_val - out_val) >= precision )
			{
				char s[40];
				sprintf(s," %lf != %lf",ans_val,out_val);
				res.status = vsWrongAnswer;
				res.comment = "Wrong number in solution's output";
				res.comment += s;
				return res;
			}
		}
	}
	res.status = vsOk;
	res.comment = "All numbers are equal";
	return res;
}

struct ValidationResult CValidator::ValidateByXmlOutput(std::string xml_res)
{
	ValidationResult res;
	
	TiXmlDocument doc(xml_res);
	if ( !doc.LoadFile() )
	{
		std::string msg = doc.ErrorDesc();
		msg += " while parsing validator's log file";
		SetFailResult(res, msg);
		return res;
	}
	TiXmlElement* root = doc.FirstChildElement("result");
	if ( !root )
	{
		SetFailResult(res,"Cannot find \"result\" tag in validator output");
		return res;
	}
	const char* oc = root->Attribute("outcome");
	if ( !oc )
	{
		SetFailResult(res,"Cannot parse \"outcome\" attribute");
		return res;
	}
	else
	{
		std::string outcome = oc;
		LowercaseString(outcome);
		if ( outcome == "accepted"  || outcome == "ok" )
		{
			res.status = vsOk;
		}
		else if ( outcome == "wrong-answer" || outcome == "wrong answer" || outcome == "wa" )
		{
			res.status = vsWrongAnswer;
		}
		else if ( outcome == "presentation-error" || outcome == "presentation error" || outcome == "pe" )
		{
			res.status = vsPresentationError;
		}
		else
		{
			res.status = vsFailure;
		}
		const char* comment = root->Attribute("comment");
		if ( comment )
		{
			res.comment = comment;
		}
		else
		{
			TiXmlNode* text = root->FirstChild();
			if ( text )
			{
				res.comment = text->Value();
			}
		}
	}
	FileDelete(xml_res);
	return res;
}

CTokenReader::CTokenReader(FILE* token_file)
{
	file = token_file;
	has = true;
	begin = end = 0;
}

bool CTokenReader::HasSymbol()
{
	return has;
}

unsigned char CTokenReader::ReadSymbol()
{
	int read;
	if ( begin >= end )
	{
		read = (int)fread(buffer,sizeof(unsigned char),BUFFER_SIZE,file);
		if ( read == 0 )
		{
			has = false;
		}
		end = read;
	}
	return buffer[begin++];
}

CTokenReader::~CTokenReader()
{}
