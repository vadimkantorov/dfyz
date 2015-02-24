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
 
#include "fileutils.hpp"

#ifdef WIN32
	const char delimiter = '\\';
#else
	const char delimiter = '/';
#endif

bool PathExists(std::string file)
{
#ifdef WIN32
	WIN32_FIND_DATA data = {0};
	HANDLE res = FindFirstFile(file.c_str(),&data);
	if ( res == INVALID_HANDLE_VALUE )
	{
		return false;
	}
	else
	{
		FindClose(res);
		return true;
	}
#else
	struct stat st;
	stat(dir,&stat);
	return ( S_ISDIR(st.st_mode) != 0 )?false:true;
#endif
}

std::string PathCombine(std::string dir, std::string file)
{
	if ( dir[dir.size()-1] == delimiter )
	{
		dir = dir.substr(0,dir.size()-1);
	}
	if ( file[0] == delimiter && (file.size() != 1) )
	{
		file = file.substr(1);
	}
	return dir + delimiter + file;
}

std::string GetSuffix(std::string file)
{
	std::string::size_type dot = file.find(".");
	if ( dot == file.npos )
	{
		return "";
	}
	else
	{
		dot++;
		return file.substr(dot);
	}
}

std::string GetBasename(std::string file)
{
	std::string::size_type delim = file.rfind(delimiter);
	if ( delim == file.npos )
	{
		delim = 0;
	}
	else
	{
		delim++;
	}
	return file.substr(delim);
}

std::string GetDirname(std::string file)
{
	std::string::size_type delim = file.rfind(delimiter);
	return file.substr(0,delim);
}

bool FileDelete(std::string file)
{
#ifdef WIN32
	return ( !DeleteFile(file.c_str()) )?false:true;
#else
	return (remove(file.c_str())==0)?false:true;
#endif
}

bool FileCopy(std::string src, std::string dest)
{
#ifdef WIN32
	return ( CopyFile(src.c_str(),dest.c_str(),FALSE) == 0 )?false:true;
#else
	bool ok = true;
	const int BUF_SIZE = 256;
	int from,to,bytes_from,bytes_to;
	char buf[BUF_SIZE];
	if ( (from = open(src.c_str(), O_RDONLY) ) == -1 )
	{
		ok = false;
	}
	if ( (to = open(dest.c_str(), O_WRONLY) ) == -1 )
	{
		ok = false;
	}
	while ( (bytes_from = read(from,buf,BUF_SIZE) )  > 0)
	{
		bytes_to = write(to,buf,bytes_from);
		if ( bytes_from != bytes_to )
		{
			ok = false;
			break;
		}
	}
	close(from);
	close(to);
	return ok;
#endif
}

unsigned int FileSize(std::string file)
{
#ifdef WIN32
	int size;
	HANDLE hfile = CreateFile(file.c_str(), GENERIC_READ, 0 , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	/*if ( hfile == INVALID_HANDLE_VALUE )
	{
		return 0;
	}*/
	size = GetFileSize(hfile,NULL);
	CloseHandle(hfile);
	return size;
#endif
}

CFileHandler::CFileHandler()
{}

void CFileHandler::PushFile(std::string file)
{
	files.push_back(file);
}

CFileHandler::~CFileHandler()
{
	for ( unsigned int i = 0; i < files.size(); i++ )
	{
		FileDelete(files[i]);
	}
}
