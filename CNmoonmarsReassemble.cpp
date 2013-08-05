#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <getopt.h>
#include <dirent.h>
#include "Common.h"
#include "HotspotCoordsWithProbability.h"
#include "PossibleHotspotsDistribution.h"

struct PartialFile {
	std::string directory;
	std::string filename;
	FILE* fileStream;
	
	int startIndex;
	int endIndex;
	int gridRes;
	int increment;
	int interval;
};

struct Params {
	std::string resultsDir;
	
	std::string inputFile;
	std::string limitsFile;
	std::string mFile;
	std::string abcdDistFile;
	std::string possibleHotspotsFile;
	std::string nonremovableHotspotsFile;
	std::string nonremovableProbFile;
};

Params DefaultParams() {
	Params params;
	
	params.resultsDir = "output/";
	
	params.inputFile = "input-observedhotspots.txt";
	params.limitsFile = "limits.txt";
	params.mFile = "M.txt";
	params.abcdDistFile = "abcdspaceprob.txt";
	params.possibleHotspotsFile = "possiblehotspots.txt";
	params.nonremovableHotspotsFile = "possiblehotspots-nonremovable.txt";
	params.nonremovableProbFile = "nonremovable-prob.txt";
	
	return params;
}

void ParseArguments(int argc, char* argv[], Params &params) {
	static struct option long_options[] =
	{
		{"resultsDir",						required_argument, NULL, 130},
		{"inputFile",					required_argument, NULL, 131},
		{"limitsFile",					required_argument, NULL, 132},
		{"abcdDistFile",				required_argument, NULL, 133},
		{"possibleHotspotsFile",		required_argument, NULL, 134},
		{"nonremovableHotspotsFile",	required_argument, NULL, 135},
		{"nonremovableProbFile",		required_argument, NULL, 136},
		{"mFile",						required_argument, NULL, 137},
		{0, 0, 0, 0}
	};
	
	int option_index;
	int c;
	while ((c = getopt_long_only(argc, argv, "", long_options, &option_index)) != -1) {
		switch (c)
		{
			case 130: params.resultsDir = optarg; break;
			case 131: params.inputFile = optarg; break;
			case 132: params.inputFile = optarg; break;
			case 133: params.abcdDistFile = optarg; break;
			case 134: params.possibleHotspotsFile = optarg; break;
			case 135: params.nonremovableHotspotsFile = optarg; break;
			case 136: params.nonremovableProbFile = optarg; break;
			case 137: params.mFile= optarg; break;
			default: 
				printf("Error: Could not parse arguments.\n");
				exit(EXIT_FAILURE);
		}
	}
}

void StandardizeDirectoryNames(Params &params) {
	StandardizeDirectoryName(params.resultsDir);
}

std::vector<std::string> GetPartialSubdirectories(std::string dirName) {
	std::vector<std::string> result;
	
	DIR* dirp = opendir(dirName.c_str());
	if (dirp == NULL) {
		printf("Error: Could not open directory: \"%s\"\n", dirName.c_str());
		exit(EXIT_FAILURE);
	}
	
	dirent* dp;
	while ((dp = readdir(dirp)) != NULL) {
		std::string subDirName = dirName + dp->d_name;
		StandardizeDirectoryName(subDirName);
		int startIndex, endIndex;
		
		if (strcmp(dp->d_name,".")!=0 && strcmp(dp->d_name,"..")!=0 &&
			sscanf(dp->d_name, "%d-%d", &startIndex, &endIndex)==2 &&
			DirectoryExists(subDirName.c_str())) {
			
			result.push_back(subDirName);
		}
	}
	
	closedir(dirp);
	
	return result;
}

std::vector<HotspotCoordsWithProbability>* CombinePossibleHotspotFiles(std::vector<std::string> partialDirs, std::string resultsDir, std::string possibleHotspotsFilename) {
	std::vector<PartialFile*> partialFiles;
	
	//
	// open files
	//
	for(std::vector<std::string>::iterator it = partialDirs.begin(); it < partialDirs.end(); it++) {
		const char* filename = (*it + possibleHotspotsFilename).c_str();
		FILE* file = fopen(filename, "r");
		if(!file) {
			printf("Error: Could not open file for reading: \"%s\"\n", filename);
			exit(EXIT_FAILURE);
		}
		
		PartialFile* pFile = new PartialFile();
		pFile->directory = *it;
		pFile->filename = filename;
		pFile->fileStream = file;
		partialFiles.push_back(pFile);
	}
	
	printf("%-30s%12s%12s%12s%12s%12s\n", "Directory", "Start Index", "End Index", "Grid Res", "Increment", "Interval");
	
	//
	// read headers
	//
	std::vector<PartialFile*>::iterator it;
	for(it = partialFiles.begin(); it < partialFiles.end(); it++) {
		PartialFile* partialFile = *it;
		
		if(fscanf(partialFile->fileStream, "!! THIS IS A PARTIAL FILE !!\n") != 0) {
			printf("Error: Could not read \"!! THIS IS A PARTIAL FILE !!\" from file: \"%s\".\n",
					partialFile->filename.c_str());
			exit(EXIT_FAILURE);
		}
		
		if(!fscanf(partialFile->fileStream, "START INDEX = %4d\n", &(partialFile->startIndex))) {
			printf("Error: Could not read startIndex from file: \"%s\".\n", partialFile->filename.c_str());
			exit(EXIT_FAILURE);
		}
		
		if(!fscanf(partialFile->fileStream, "END   INDEX = %4d\n", &(partialFile->endIndex))) {
			printf("Error: Could not read endIndex from file: \"%s\".\n", partialFile->filename.c_str());
			exit(EXIT_FAILURE);
		}
		
		if(!fscanf(partialFile->fileStream, "GRID  RES   = %4d\n", &(partialFile->gridRes))) {
			printf("Error: Could not read gridRes from file: \"%s\".\n", partialFile->filename.c_str());
			exit(EXIT_FAILURE);
		}
		
		if(!fscanf(partialFile->fileStream, "INCREMENT   = %4d\n", &(partialFile->increment))) {
			printf("Error: Could not read increment from file: \"%s\".\n", partialFile->filename.c_str());
			exit(EXIT_FAILURE);
		}
		
		if(!fscanf(partialFile->fileStream, "INTERVAL    = %4d\n\n", &(partialFile->interval))) {
			printf("Error: Could not read interval from file: \"%s\".\n", partialFile->filename.c_str());
			exit(EXIT_FAILURE);
		}
		
		if(fscanf(partialFile->fileStream, "PROBABILITIES ARE NOT NORMALIZED\n\n") != 0) {
			printf("Error: Could not read \"PROBABILITIES ARE NOT NORMALIZED\" from file: \"%s\".\n",
				   partialFile->filename.c_str());
			exit(EXIT_FAILURE);
		}
		
		printf("%-30s%12d%12d%12d%12d%12d\n", (partialFile->directory).c_str(), partialFile->startIndex,
			   partialFile->endIndex, partialFile->gridRes, partialFile->increment, partialFile->interval);
	}
	printf("\n");
	
	//
	// Check parameters for compatability
	//
	for(it = partialFiles.begin(); it < partialFiles.end(); it++) {
		PartialFile* partialFile = *it;
		PartialFile* initFile = *(partialFiles.begin());
		if(partialFile->gridRes * initFile->increment != initFile->gridRes * partialFile->increment) {
			printf("Error: Incompatible files:\n");
			printf("%s has gridRes = %d, increment = %d\n", initFile->filename.c_str(),
				initFile->gridRes, initFile->increment);
			printf("%s has gridRes = %d, increment = %d\n", partialFile->filename.c_str(),
				   partialFile->gridRes, partialFile->increment);
			exit(EXIT_FAILURE);
		}
	}
	
	//
	// Read possible hotspots, and verify matches
	//
	std::vector<HotspotCoordsWithProbability>* possibleHotspots;
	possibleHotspots = new std::vector<HotspotCoordsWithProbability>();
	int count = 0;
	while (true) {
		// check if end of files is reached, and verify end of all files
		// is reached simultaneously
		PartialFile* firstFile = *(partialFiles.begin());
		bool filesEnded = feof(firstFile->fileStream);
		for(it = partialFiles.begin(); it < partialFiles.end(); it++) {
			PartialFile* partialFile = *it;
			if(feof(partialFile->fileStream) != filesEnded) {
				printf("Error: Files have differing number of lines:\n");
				printf("%s\n", firstFile->filename.c_str());
				printf("%s\n", partialFile->filename.c_str());
				exit(EXIT_FAILURE);
			}
		}
		if (filesEnded)
			break;
		
		// read one coord from each of the files, and verify match
		HotspotCoordsWithProbability coord;
		bool readCoords = false;
		bool readProb = false;
		for(it = partialFiles.begin(); it < partialFiles.end(); it++) {
			PartialFile* partialFile = *it;
			HotspotCoordsWithProbability newCoord;
			if(!fscanf(partialFile->fileStream, "%6hd", &(newCoord.moonLat))) {
				printf("Error: Could not read moonLat from file: \"%s\".\n",
						partialFile->filename.c_str());
				exit(EXIT_FAILURE);
			}
			if(!fscanf(partialFile->fileStream, "%6hd", &(newCoord.moonLong))) {
				printf("Error: Could not read moonLong from file: \"%s\".\n",
						partialFile->filename.c_str());
				exit(EXIT_FAILURE);
			}
			if(!fscanf(partialFile->fileStream, "%6hd", &(newCoord.marsLat))) {
				printf("Error: Could not read marsLat from file: \"%s\".\n",
						partialFile->filename.c_str());
				exit(EXIT_FAILURE);
			}
			if(!fscanf(partialFile->fileStream, "%6hd", &(newCoord.marsLong))) {
				printf("Error: Could not read marsLong from file: \"%s\".\n",
						partialFile->filename.c_str());
				exit(EXIT_FAILURE);
			}
			if(!fscanf(partialFile->fileStream, "%46Le\n", &(newCoord.prob))) {
				printf("Error: Could not read prob from file: \"%s\".\n",
						partialFile->filename.c_str());
				exit(EXIT_FAILURE);
			}
			
			if (readCoords) {
				if (coord != newCoord) {
					printf("Error: Coordinate mismatch in coordinate %d while reading file \"%s\":\n",
							count+1, partialFile->filename.c_str());
					printf("Old: %s.\n", coord.ToString().c_str());
					printf("New: %s.\n",  newCoord.ToString().c_str());
					exit(EXIT_FAILURE);
				}
			} else {
				coord.moonLat = newCoord.moonLat;
				coord.moonLong = newCoord.moonLong;
				coord.marsLat = newCoord.marsLat;
				coord.marsLong = newCoord.marsLong;
				readCoords = true;
			}
			
			if (count+1 >= partialFile->startIndex &&
				count+1 <= partialFile->endIndex) {
				if (readProb) {
					if (coord.prob != newCoord.prob) {
						printf("Error: Probability mismatch in coordinate %d while reading file \"%s\":\n",
								count+1, partialFile->filename.c_str());
						printf("Old: %s.\n", coord.ToString().c_str());
						printf("New: %s.\n",  newCoord.ToString().c_str());
						exit(EXIT_FAILURE);
					}
				} else {
					coord.prob = newCoord.prob;
					readProb = true;
				}
			}
		}
		if (!readCoords) {
			printf("Error: Coordinate %d was not read.\n", count+1);
			exit(EXIT_FAILURE);
		}
		if (!readProb) {
			printf("Error: Probability for coordinate %d was not found in any of the files.\n", count+1);
			exit(EXIT_FAILURE);
		}
		
		possibleHotspots->push_back(coord);
		count++;
	}
	
	if (count != (int)possibleHotspots->size()) {
		printf("Error: Coordinate count, %d, does not match size of coordinate vector, %zu.\n\n",
				count, possibleHotspots->size());
		exit(EXIT_FAILURE);
	}
	
	//
	// Normalize
	//
	PossibleHotspotsDistribution::Normalize(possibleHotspots);
	
	//
	// Write results to file
	//
	std::string filename = resultsDir + possibleHotspotsFilename;
	FILE* file = fopen(filename.c_str(), "w");
	if(!file) {
		printf("Error: Could not open file for writing: \"%s\"\n", filename.c_str());
		exit(EXIT_FAILURE);
	}
	
	for(std::vector<HotspotCoordsWithProbability>::iterator it = possibleHotspots->begin(); it<possibleHotspots->end(); it++){
		HotspotCoordsWithProbability coords = *it;
		fprintf(file, "%s\n", coords.ToString().c_str());
	}
	
	fclose(file);	
	printf("Printed combined distribution with %zu hotspots to file: \"%s\".\n\n", possibleHotspots->size(), filename.c_str());
	
	//
	// Close files
	//
	for(it = partialFiles.begin(); it < partialFiles.end(); it++) {
		PartialFile* partialFile = *it;
		fclose(partialFile->fileStream);
		delete(partialFile);
	}
	
	return possibleHotspots;
}

int GetFileBytes(std::string filename, char* &bytes) {
	// open the file
	FILE* file = fopen(filename.c_str(), "r");
	if(!file) {
		printf("Error: Could not open file for reading: \"%s\"\n", filename.c_str());
		exit(EXIT_FAILURE);
	}
	
	// obtain file size:
	fseek (file , 0 , SEEK_END);
	int fileSize = ftell (file);
	rewind(file);
	
	// allocate memory to contain the whole file:
	bytes = (char*) malloc (sizeof(char)*(fileSize+1));
	if(!bytes) {
		printf("Error: Could not malloc bytes for reading \"%s\".\n", filename.c_str());
		exit(EXIT_FAILURE);
	}
	
	bytes[fileSize] = '\0';
	
	// copy the file into the buffer:
	int result = fread (bytes, 1, fileSize, file);
	if(result != fileSize) {
		printf("Error: Could not read all %d bytes from file \"%s\".\n", fileSize, filename.c_str());
		exit(EXIT_FAILURE);
	}
	
	fclose (file);
	return fileSize;
}

void VerifyAndCopyMatchingFiles(std::vector<std::string> directories, std::string resultsDir, std::string filename) {
	char* bytes;
	std::string firstFullFileName = *(directories.begin())+filename;
	int fileSize = GetFileBytes(firstFullFileName, bytes);
	
	for (std::vector<std::string>::iterator dir=directories.begin(); dir<directories.end(); dir++) {
		char* bytesCurr;
		std::string fullFileName = *dir+filename;
		int fileSizeCurr = GetFileBytes(fullFileName, bytesCurr);
		
		if(fileSize!=fileSizeCurr) {
			printf("Error: Files do not have the same size:\n");
			printf("%s\n", firstFullFileName.c_str());
			printf("%s\n",  fullFileName.c_str());
			exit(EXIT_FAILURE);
		}
		
		if(strcmp(bytes, bytesCurr) != 0) {
			printf("Error: Files do not match:\n");
			printf("%s\n", firstFullFileName.c_str());
			printf("%s\n", fullFileName.c_str());
			exit(EXIT_FAILURE);
		}
		
		free(bytesCurr);
	}
	
	std::string outFileName = resultsDir + filename;
	FILE* file = fopen(outFileName.c_str(), "w");
	if(!file) {
		printf("Error: Could not open file for writing: \"%s\"\n", filename.c_str());
		exit(EXIT_FAILURE);
	}
	
	fwrite(bytes, 1, fileSize, file);
	
	fclose(file);
	printf("Output file: \"%s\".\n", outFileName.c_str());
	
	free(bytes);
}

int main(int argc, char* argv[]) {
	Params params = DefaultParams();	
	ParseArguments(argc, argv, params);
	StandardizeDirectoryNames(params);
	
	printf("==========================================================================================\n");
	
	std::vector<std::string> partialDirs = GetPartialSubdirectories(params.resultsDir);
	if (partialDirs.empty()) {
		printf("Error: No directories found containg partial results.\n");
		exit(EXIT_FAILURE);
	}
	
	std::vector<HotspotCoordsWithProbability>* possibleHotspotsVec;
	possibleHotspotsVec = CombinePossibleHotspotFiles(partialDirs, params.resultsDir, params.possibleHotspotsFile);
	PossibleHotspotsDistribution possibleHotspots(possibleHotspotsVec);
	
	VerifyAndCopyMatchingFiles(partialDirs, params.resultsDir, params.inputFile);
	VerifyAndCopyMatchingFiles(partialDirs, params.resultsDir, params.limitsFile);
	VerifyAndCopyMatchingFiles(partialDirs, params.resultsDir, params.abcdDistFile);
	
	//
	// Determine nonremovable hotspots and probability
	//
	ObservedHotspots observedHotspots(params.resultsDir + params.inputFile);
	AbcdSpaceLimits limits(observedHotspots, false);
	
	printf("\nFinding nonremovable possible hotspots:\n");
	PossibleHotspotsDistribution nonremovableHotspots(limits, true);
	nonremovableHotspots.PrintToFile(params.resultsDir + params.nonremovableHotspotsFile, false);
	Double accumProb = possibleHotspots.GetTotalProbability(nonremovableHotspots);
	
	std::string filename = params.resultsDir + params.nonremovableProbFile;
	FILE* file = fopen(filename.c_str(), "w");
	if(!file) {
		printf("Error: Could not open file for writing: \"%s\"\n", filename.c_str());
		exit(EXIT_FAILURE);
	}
	
	fprintf(file, "Probability of getting a nonremovable & non-removing point next month:\n");
	fprintf(file, "%.36Lg%%\n", 100*accumProb);
	fprintf(file, "Probability of getting a removable & removing point next month:\n");
	fprintf(file, "%.36Lg%%\n", 100*(1-accumProb));
	
	fclose(file);
	printf("\nPrinted nonremovable & removable probabilities to file: \"%s\".\n", filename.c_str());
	
	printf("\nProbability of getting a nonremovable & non-removing point next month:\n");
	printf("%.36Lg%%\n", 100*accumProb);
	printf("Probability of getting a removable & removing point next month:\n");
	printf("%.36Lg%%\n", 100*(1-accumProb));
	//
	// End nonremovable hotspots and probability
	//
	
	delete(possibleHotspotsVec);
	
	printf("\nResults reassembled successfully.\n");
	
	return EXIT_SUCCESS;
}
