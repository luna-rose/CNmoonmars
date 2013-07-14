#include "Common.h"

void StandardizeDirectoryName(std::string &dirName) {
	if(dirName=="")
		dirName = "./";			
	if(*(dirName.end()-1)!='/')
		dirName += '/';
}