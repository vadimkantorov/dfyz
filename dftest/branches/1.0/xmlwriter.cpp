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
 
#include "xmlwriter.hpp"

void CXmlWriter::XmlWriteVersion()
{
	file << "<?xml version=\"1.0\" encoding=\"cp866\"?>" << std::endl;
}

void CXmlWriter::XmlWriteStylesheet(std::string style_file)
{
	file << "<?xml-stylesheet type=\"text/xsl\" href=\"";
	XmlWriteBody(style_file);
	file << "\"?>" << std::endl;
}

CXmlWriter::CXmlWriter(std::string filename)
{
	need_closing = false;
	body_written = false;

	file.open(filename.c_str(), std::ios_base::out | std::ios_base::binary);
	if ( !file )
	{
		std::string message = "Cannot open " + GetBasename(filename) + " for XML writing";
		throw DftestException(message, false);
	}
}

CXmlWriter::~CXmlWriter()
{
	XmlCloseAllTags();
}

void CXmlWriter::XmlWriteBody(std::string text)
{
	if ( need_closing )
	{
		file << ">";
	}
	for ( unsigned int i = 0; i < text.size(); i++ )
	{
		switch ( text[i] )
		{
		case '\'':
			file << "&apos;";
			break;
		case '"':
			file << "&quot;";
			break;
		case '<':
			file << "&lt;";
			break;
		case '>':
			file << "&gt;";
			break;
		case '&':
			file << "&amp;";
			break;
		default:
			file << text[i];
		}
	}
	need_closing = false;
	body_written = true;
}

void CXmlWriter::XmlOpenTag(std::string name)
{
	if ( need_closing )
	{
		file << ">" << std::endl;
	}
	body_written = false;
	tags.push(name);
	file << "<" << name;
	need_closing = true;
}

void CXmlWriter::XmlWriteAttribute(std::string name, std::string value)
{
	file << " " << name << "=\"" << value << "\"";
}

bool CXmlWriter::XmlCloseTag()
{
	if ( need_closing )
	{
		file << ">";
	}
	if ( !tags.empty() )
	{
		file << "</" << tags.top() << ">" << std::endl;
		tags.pop();
		need_closing = false;
		return true;
	}
	else
	{
		return false;
	}
}

void CXmlWriter::XmlCloseAllTags()
{
	while ( XmlCloseTag() );
}
