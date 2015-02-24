#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "common.hpp"
#include <string>
#include <vector>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif

// Copy file from source to destination
bool FileCopy(std::string src, std::string dest);
// Remove file
bool FileDelete(std::string file);

// Concatenate directory and file, appending OS-specific string delimiter in the middle
std::string PathCombine(std::string dir, std::string file);
// Get file suffix(extension in Windows)
std::string GetSuffix(std::string file);
// Get file basename
std::string GetBasename(std::string file);
// Get directory name
std::string GetDirname(std::string file);
// Check if file/directory exists
bool PathExists(std::string file);
// Get size of a file in bytes (-1 in case of failure)
unsigned int FileSize(std::string file);

class CFileHandler
{
private:
	std::vector<std::string> files;
public:
	CFileHandler();
	void PushFile(std::string file);
	~CFileHandler();
};

#endif
