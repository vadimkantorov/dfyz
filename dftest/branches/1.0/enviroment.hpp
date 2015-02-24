#ifndef ENVIROMENT_H
#define ENVIROMENT_H

#include <string>
#include <map>

struct CompilerInfo
{
	std::string Name;
	std::string Executable;
	std::string Arguments;
};

typedef std::map<std::string,CompilerInfo> Compilers;

extern std::string runDir;
extern std::string problemDir;
extern struct Problem prob;
extern struct ConfigValues values;
extern Compilers comp_map;
extern struct SolutionFiles files;
#endif



