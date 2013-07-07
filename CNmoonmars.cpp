#include <fstream>
#include <cstdlib>
#include "ObservedHotspots.h"
#include "Common.h"
#include "AbcdSpaceLimits.h"
#include "AbcdSpaceProbabilityDistribution.h"
#include "PossibleHotspotsDistribution.h"
#ifdef using_parallel
	#include <omp.h>
#endif

int main(int argc, char* argv[]) {
	int gridRes = 5;
	int increment = 1;
#ifdef using_parallel
	int interval = omp_get_max_threads();
#else
	int interval = 1;
#endif

	if (argc >= 2)
		gridRes = atoi(argv[1]);
	if (argc >= 3)
		increment = atoi(argv[2]);
	if (argc >= 4)
		interval = atoi(argv[3]);
	if (argc >= 5) {
		printf("Too many arguments!");
		return EXIT_FAILURE;
	}
	
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
