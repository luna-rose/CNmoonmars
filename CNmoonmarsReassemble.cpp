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
	
	std::string inputFilename;
	std::string limitsFilename;
	std::string abcdDistFilename;
	std::string possibleHotspotsFilename;
};

Params DefaultParams() {
	Params params;
	
	params.resultsDir = "output/";
	
	params.inputFilename = "input-observedhotspots.txt";
	params.limitsFilename = "limits.txt";
	params.abcdDistFilename = "abcdspaceprob.txt";
	params.possibleHotspotsFilename = "possiblehotspots.txt";
	
	return params;
}

void ParseArguments(int argc, char* argv[], Params &params) {
	static struct option long_options[] =
	{
		{"resultsDir",					required_argument, NULL, 130},
		{"inputFilename",				required_argument, NULL, 131},
		{"limitsFilename",				required_argument, NULL, 132},
		{"abcdDistFilename",			required_argument, NULL, 133},
		{"possibleHotspotsFilename",	required_argument, NULL, 134},
		{0, 0, 0, 0}
	};
	
	int option_index;
	int c;
	while ((c = getopt_long_only(argc, argv, "", long_options, &option_index)) != -1) {
		switch (c)
		{
			case 130: params.resultsDir = optarg; break;
			case 131: params.inputFilename = optarg; break;
			case 132: params.inputFilename = optarg; break;
			case 133: params.abcdDistFilename = optarg; break;
			case 134: params.possibleHotspotsFilename = optarg; break;
			default: 
				fprintf(stderr, "Error: Could not parse arguments.\n");
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
		fprintf(stderr, "Error: Could not open directory: \"%s\"\n", dirName.c_str());
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

void CombinePossibleHotspotFiles(std::vector<std::string> partialDirs, std::string resultsDir, std::string possibleHotspotsFilename) {
	std::vector<PartialFile*> partialFiles;
	
	if (partialDirs.empty()) {
		fprintf(stderr, "Error: No directories found containg partial results.\n");
		exit(EXIT_FAILURE);
	}
	
	//
	// open files
	//
	for(std::vector<std::string>::iterator it = partialDirs.begin(); it < partialDirs.end(); it++) {
		const char* filename = (*it + possibleHotspotsFilename).c_str();
		FILE* file = fopen(filename, "r");
		if(!file) {
			fprintf(stderr, "Error: Could not open file for reading: \"%s\"\n", filename);
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
			fprintf(stderr, "Error: Could not read \"!! THIS IS A PARTIAL FILE !!\" from file: \"%s\".\n", 
					partialFile->filename.c_str());
			exit(EXIT_FAILURE);
		}
		
		if(!fscanf(partialFile->fileStream, "START INDEX = %4d\n", &(partialFile->startIndex))) {
			fprintf(stderr, "Error: Could not read startIndex from file: \"%s\".\n", partialFile->filename.c_str());
			exit(EXIT_FAILURE);
		}
		
		if(!fscanf(partialFile->fileStream, "END   INDEX = %4d\n", &(partialFile->endIndex))) {
			fprintf(stderr, "Error: Could not read endIndex from file: \"%s\".\n", partialFile->filename.c_str());
			exit(EXIT_FAILURE);
		}
		
		if(!fscanf(partialFile->fileStream, "GRID  RES   = %4d\n", &(partialFile->gridRes))) {
			fprintf(stderr, "Error: Could not read gridRes from file: \"%s\".\n", partialFile->filename.c_str());
			exit(EXIT_FAILURE);
		}
		
		if(!fscanf(partialFile->fileStream, "INCREMENT   = %4d\n", &(partialFile->increment))) {
			fprintf(stderr, "Error: Could not read increment from file: \"%s\".\n", partialFile->filename.c_str());
			exit(EXIT_FAILURE);
		}
		
		if(!fscanf(partialFile->fileStream, "INTERVAL    = %4d\n\n", &(partialFile->interval))) {
			fprintf(stderr, "Error: Could not read interval from file: \"%s\".\n", partialFile->filename.c_str());
			exit(EXIT_FAILURE);
		}
		
		if(fscanf(partialFile->fileStream, "PROBABILITIES ARE NOT NORMALIZED\n\n") != 0) {
			fprintf(stderr, "Error: Could not read \"PROBABILITIES ARE NOT NORMALIZED\" from file: \"%s\".\n", 
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
			fprintf(stderr, "Error: Incompatible files:\n");
			fprintf(stderr, "%s has gridRes = %d, increment = %d\n", initFile->filename.c_str(),
				initFile->gridRes, initFile->increment);
			fprintf(stderr, "%s has gridRes = %d, increment = %d\n", partialFile->filename.c_str(),
				   partialFile->gridRes, partialFile->increment);
			exit(EXIT_FAILURE);
		}
	}
	
	//
	// Read possible hotspots, and verify matches
	//
	std::vector <HotspotCoordsWithProbability> possibleHotspots;
	int count = 0;
	while (true) {
		// check if end of files is reached, and verify end of all files
		// is reached simultaneously
		PartialFile* firstFile = *(partialFiles.begin());
		bool filesEnded = feof(firstFile->fileStream);
		for(it = partialFiles.begin(); it < partialFiles.end(); it++) {
			PartialFile* partialFile = *it;
			if(feof(partialFile->fileStream) != filesEnded) {
				fprintf(stderr, "Error: Files have differing number of lines:\n");
				fprintf(stderr, "%s\n", firstFile->filename.c_str());
				fprintf(stderr, "%s\n", partialFile->filename.c_str());
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
				fprintf(stderr, "Error: Could not read moonLat from file: \"%s\".\n", 
						partialFile->filename.c_str());
				exit(EXIT_FAILURE);
			}
			if(!fscanf(partialFile->fileStream, "%6hd", &(newCoord.moonLong))) {
				fprintf(stderr, "Error: Could not read moonLong from file: \"%s\".\n", 
						partialFile->filename.c_str());
				exit(EXIT_FAILURE);
			}
			if(!fscanf(partialFile->fileStream, "%6hd", &(newCoord.marsLat))) {
				fprintf(stderr, "Error: Could not read marsLat from file: \"%s\".\n", 
						partialFile->filename.c_str());
				exit(EXIT_FAILURE);
			}
			if(!fscanf(partialFile->fileStream, "%6hd", &(newCoord.marsLong))) {
				fprintf(stderr, "Error: Could not read marsLong from file: \"%s\".\n", 
						partialFile->filename.c_str());
				exit(EXIT_FAILURE);
			}
			if(!fscanf(partialFile->fileStream, "%46Le\n", &(newCoord.prob))) {
				fprintf(stderr, "Error: Could not read prob from file: \"%s\".\n", 
						partialFile->filename.c_str());
				exit(EXIT_FAILURE);
			}
			
			if (readCoords) {
				if (coord.moonLat != newCoord.moonLat || coord.moonLong != newCoord.moonLong ||
					coord.marsLat != newCoord.marsLat || coord.marsLong != newCoord.marsLong) {
					fprintf(stderr, "Error: Coordinate mismatch in coordinate %d while reading file \"%s\":\n",
							count+1, partialFile->filename.c_str());
					fprintf(stderr, "Old: %s.\n", coord.ToString().c_str());
					fprintf(stderr, "New: %s.\n",  newCoord.ToString().c_str());
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
						fprintf(stderr, "Error: Probability mismatch in coordinate %d while reading file \"%s\":\n",
								count+1, partialFile->filename.c_str());
						fprintf(stderr, "Old: %s.\n", coord.ToString().c_str());
						fprintf(stderr, "New: %s.\n",  newCoord.ToString().c_str());
						exit(EXIT_FAILURE);
					}
				} else {
					coord.prob = newCoord.prob;
					readProb = true;
				}
			}
		}
		if (!readCoords) {
			fprintf(stderr, "Error: Coordinate %d was not read.\n", count+1);
			exit(EXIT_FAILURE);
		}
		if (!readProb) {
			fprintf(stderr, "Error: Probability for coordinate %d was not found in any of the files.\n", count+1);
			exit(EXIT_FAILURE);
		}
		
		possibleHotspots.push_back(coord);
		count++;
	}
	
	if (count != (int)possibleHotspots.size()) {
		fprintf(stderr, "Error: Coordinate count, %d, does not match size of coordinate vector, %zu.\n\n", 
				count, possibleHotspots.size());
		exit(EXIT_FAILURE);
	}
	
	//
	// Normalize
	//
	PossibleHotspotsDistribution::Normalize(&possibleHotspots);
	
	//
	// Write results to file
	//
	std::string filename = resultsDir + possibleHotspotsFilename;
	FILE* file = fopen(filename.c_str(), "w");
	if(!file) {
		fprintf(stderr, "Error: Could not open file for writing: \"%s\"\n", filename.c_str());
		exit(EXIT_FAILURE);
	}
	
	for(std::vector<HotspotCoordsWithProbability>::iterator it = possibleHotspots.begin(); it<possibleHotspots.end(); it++){
		HotspotCoordsWithProbability coords = *it;
		fprintf(file, "%s\n", coords.ToString().c_str());
	}
	
	fclose(file);	
	printf("Printed combined with with %zu hotspots to file: \"%s\".\n", possibleHotspots.size(), filename.c_str());
	
	//
	// Close files
	//
	for(it = partialFiles.begin(); it < partialFiles.end(); it++) {
		PartialFile* partialFile = *it;
		fclose(partialFile->fileStream);
		delete(partialFile);
	}
}

int main(int argc, char* argv[]) {
	Params params = DefaultParams();	
	ParseArguments(argc, argv, params);
	StandardizeDirectoryNames(params);
	
	std::vector<std::string> partialDirs = GetPartialSubdirectories(params.resultsDir);
	
	printf("==========================================================================================\n");
	
	CombinePossibleHotspotFiles(partialDirs, params.resultsDir, params.possibleHotspotsFilename);
	
	return EXIT_SUCCESS;
}
