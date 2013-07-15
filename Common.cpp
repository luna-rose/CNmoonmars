#include <sys/stat.h>
#include "Common.h"

void StandardizeDirectoryName(std::string &dirName) {
	if(dirName=="")
		dirName = "./";			
	if(*(dirName.end()-1)!='/')
		dirName += '/';
}

bool DirectoryExists(const char* dirName) {
	struct stat sb;	
	return (stat(dirName, &sb) == 0 && S_ISDIR(sb.st_mode));
}