#include <fstream>
#include <cstdlib>
#include <getopt.h>
#include "ObservedHotspots.h"
#include "Common.h"
#include "AbcdSpaceLimits.h"
#include "AbcdSpaceProbabilityDistribution.h"
#include "PossibleHotspotsDistribution.h"
#ifdef using_parallel
	#include <omp.h>
#endif

void ParseArguments(int argc, char* argv[], int &gridRes, int &increment, int &interval) {
	static struct option long_options[] =
	{
		{"gridres",    required_argument, NULL, 'g'},
		{"increment",  required_argument, NULL, 'c'},
		{"interval",   required_argument, NULL, 't'},
		{0, 0, 0, 0}
	};
	
	int option_index;
	int c;
	while ((c = getopt_long_only(argc, argv, "", long_options, &option_index)) != -1) {
		switch (c)
		{
			case 'g':
				gridRes = atoi(optarg);
				break;
			case 'c':
				increment = atoi(optarg);
				break;
			case 't':
				interval = atoi(optarg);
				break;
			case '?':
			default:
				fprintf (stderr, "Error: Could not parse arguments.\n");
				exit(EXIT_FAILURE);
		}
	}
}

int main(int argc, char* argv[]) {
	int gridRes = 5;
	int increment = 1;
#ifdef using_parallel
	int interval = omp_get_max_threads();
#else
	int interval = 1;
#endif
	
	ParseArguments(argc, argv, gridRes, increment, interval);
	
	printf("===============================================================\n");

#ifdef using_parallel
	printf("Number of cores:                %d\n", omp_get_num_procs());
	printf("Max number of OpenMP threads:   %d\n\n", omp_get_max_threads());
#endif
	
	printf("Grid resolution:                %d\n", gridRes);
	printf("Grid increment:                 %d\n", increment);
	printf("Abcd space chunking interval:   %d\n\n", interval);
	
	std::string dataDir = "data/";
	std::string statusDir = "status/";
	std::string outputDir = "output/";
	
	std::string inputFile = "input-observedhotspots.txt";
	std::string limitFile = "limits.txt";
	std::string abcdDistFile = "abcdspaceprob.txt";
	std::string possibleHotspotsFile = "possiblehotspots.txt";
	
	std::string infile = dataDir+inputFile;
	std::string outfile = outputDir+inputFile;
	std::ifstream src(infile.c_str());
	std::ofstream dst(outfile.c_str());
	dst << src.rdbuf();
	printf("Copied input file to \"%s\".\n\n", outfile.c_str());
	
	ObservedHotspots observedHotspots(infile);
	observedHotspots.Iterate(PrintCoord, NULL);
	printf("\n");
	
	AbcdSpaceLimits limits(observedHotspots);
	limits.PrintToFile(outputDir+limitFile);
	limits.PrintToFile(stdout);
	printf("\n");
	
	AbcdSpaceProbabilityDistribution abcdDist(observedHotspots, limits, 1, 5);
	abcdDist.PrintToFile(outputDir+abcdDistFile);
	printf("\n");
	
	PossibleHotspotsDistribution possibleHotspots(observedHotspots, limits, gridRes, increment, interval, statusDir);
	possibleHotspots.PrintToFile(outputDir+possibleHotspotsFile);
	
	return EXIT_SUCCESS;
}
