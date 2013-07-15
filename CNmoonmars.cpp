#include <fstream>
#include <cstdlib>
#include <getopt.h>
#include <sys/stat.h>
#include "ObservedHotspots.h"
#include "Common.h"
#include "AbcdSpaceLimits.h"
#include "AbcdSpaceProbabilityDistribution.h"
#include "PossibleHotspotsDistribution.h"
#ifdef using_parallel
	#include <omp.h>
#endif

struct Params {
	int gridRes;
	int increment;
	int interval;
	
	int startIndex;
	int endIndex;
	
	std::string dataDir;
	std::string statusDir;
	std::string outputDir;
	
	std::string inputFile;
	std::string limitsFile;
	std::string abcdDistFile;
	std::string possibleHotspotsFile;
};

Params DefaultParams() {
	Params params;
	
	params.gridRes = 5;
	params.increment = 1;
	
#ifdef using_parallel
	params.interval = omp_get_max_threads();
#else
	params.interval = 1;
#endif
	
	params.startIndex = 0;
	params.endIndex = 0;
	
	params.dataDir = "data/";
	params.outputDir = "output/";
	params.statusDir = "status/";
	
	params.inputFile = "input-observedhotspots.txt";
	params.limitsFile = "limits.txt";
	params.abcdDistFile = "abcdspaceprob.txt";
	params.possibleHotspotsFile = "possiblehotspots.txt";
	
	return params;
}

void ParseArguments(int argc, char* argv[], Params &params) {
	static struct option long_options[] =
	{
		{"gridRes",					required_argument, NULL, 'g'},
		{"increment",				required_argument, NULL, 'c'},
		{"interval",				required_argument, NULL, 't'},
		{"startIndex",				required_argument, NULL, 's'},
		{"endIndex",				required_argument, NULL, 'e'},
		{"dataDir",					required_argument, NULL, 128},
		{"statusDir",				required_argument, NULL, 129},
		{"outputDir",				required_argument, NULL, 130},
		{"inputFile",				required_argument, NULL, 131},
		{"limitsFile",				required_argument, NULL, 132},
		{"abcdDistFile",			required_argument, NULL, 133},
		{"possibleHotspotsFile",	required_argument, NULL, 134},
		{0, 0, 0, 0}
	};
	
	int option_index;
	int c;
	while ((c = getopt_long_only(argc, argv, "", long_options, &option_index)) != -1) {
		switch (c)
		{
			case 'g': params.gridRes = atoi(optarg); break;
			case 'c': params.increment = atoi(optarg); break;
			case 't': params.interval = atoi(optarg); break;
			case 's': params.startIndex = atoi(optarg); break;
			case 'e': params.endIndex = atoi(optarg); break;
			case 128: params.dataDir = optarg; break;
			case 129: params.statusDir = optarg; break;
			case 130: params.outputDir = optarg; break;
			case 131: params.inputFile = optarg; break;
			case 132: params.inputFile = optarg; break;
			case 133: params.abcdDistFile = optarg; break;
			case 134: params.possibleHotspotsFile = optarg; break;
			default: 
				printf("Error: Could not parse arguments.\n");
				exit(EXIT_FAILURE);
		}
	}
}

void StandardizeDirectoryNames(Params &params) {
	StandardizeDirectoryName(params.dataDir);
	StandardizeDirectoryName(params.outputDir);
	StandardizeDirectoryName(params.statusDir);
}

void MakeDirectory(std::string dirName) {
	if(!DirectoryExists(dirName.c_str())) {
		mode_t mode = S_IRWXU; 
		if(mkdir(dirName.c_str(), mode) != 0) {
			printf("Error: Could not create directory \"%s\".\n", dirName.c_str());
			exit(EXIT_FAILURE);
		}
	}
}

void MakeDirectoryRecursive(std::string dirName) {
	if(dirName == "" || dirName == "/")
		return;
	
	int parentSlashPos = dirName.rfind('/',dirName.length()-2);
	std::string parentDir = dirName.substr(0,parentSlashPos+1);
	
	MakeDirectoryRecursive(parentDir);
	MakeDirectory(dirName);
}

void ModifyDirectoryName(std::string &dirName, int start, int end) {
	char buff[2048];
	sprintf(buff, "%s%04d-%04d/", dirName.c_str(), start, end);
	dirName = buff;
}

void PrintCoord(HotspotCoordsWithDate coord, void* data) {
	printf("%s\n", coord.ToString().c_str());
}

int main(int argc, char* argv[]) {
	Params params = DefaultParams();	
	ParseArguments(argc, argv, params);
	PossibleHotspotsDistribution::ValidateIndexLimits(params.startIndex, params.endIndex);
	StandardizeDirectoryNames(params);
	
	printf("===============================================================\n");
	
#ifdef using_parallel
	printf("Number of cores:                %4d\n", omp_get_num_procs());
	printf("Max number of OpenMP threads:   %4d\n\n", omp_get_max_threads());
#endif
	
	printf("Grid resolution:                %4d\n", params.gridRes);
	printf("Grid increment:                 %4d\n", params.increment);
	printf("Abcd space chunking interval:   %4d\n\n", params.interval);
	
	std::string infile = params.dataDir + params.inputFile;
	printf("Input file is \"%s\".\n", infile.c_str());
	
	ObservedHotspots observedHotspots(infile);
	observedHotspots.Iterate(PrintCoord, NULL);
	printf("\n");
	
	AbcdSpaceLimits limits(observedHotspots);
	limits.PrintToFile(stdout);
	printf("\n");
	
	printf("Checking whether to generate a partial file, based on # of possible hotspots.\n");
	PossibleHotspotsDistribution::AdjustStartEndIndices(limits, params.startIndex, params.endIndex);
	bool isPartial = PossibleHotspotsDistribution::IsPartial(params.startIndex, params.endIndex);
	printf("\n");
	
	if(isPartial)
		ModifyDirectoryName(params.outputDir, params.startIndex, params.endIndex);
	
	MakeDirectoryRecursive(params.outputDir + params.statusDir);
	
	if(isPartial) {
		printf("GENERATING PARTIAL FILE\n");
		printf("Start index:                    %4d\n", params.startIndex);
		printf("End index:                      %4d\n\n", params.endIndex);
	}
	
	std::string outfile = params.outputDir + params.inputFile;
	std::ifstream src(infile.c_str());
	std::ofstream dst(outfile.c_str());
	dst << src.rdbuf();
	printf("Copied input file to \"%s\".\n", outfile.c_str());
	
	limits.PrintToFile(params.outputDir + params.limitsFile);
	printf("\n");
	
	AbcdSpaceProbabilityDistribution abcdDist(observedHotspots, limits, 1, 5);
	abcdDist.PrintToFile(params.outputDir + params.abcdDistFile);
	printf("\n");

	PossibleHotspotsDistribution possibleHotspots(observedHotspots, limits, params.gridRes, params.increment, params.interval, 
												  params.outputDir + params.statusDir, params.startIndex, params.endIndex);
	possibleHotspots.PrintToFile(params.outputDir + params.possibleHotspotsFile);
	
	return EXIT_SUCCESS;
}
