#include <fstream>
#include <cstdlib>
#include "ObservedHotspots.h"
#include "AbcdSpaceLimits.h"
#include "AbcdSpaceProbabilityDistribution.h"
#include "PossibleHotspotsDistribution.h"
#ifdef using_parallel
	#include <omp.h>
#endif

void PrintCoord(HotspotCoordsWithDate coord, void* data) {
	printf("%5d, %5d, %5d, %5d, %5d, %5d\n", 
		   coord.month.value, 
		   coord.year,
		   coord.moonLat,
		   coord.moonLong,
		   coord.marsLat,
		   coord.marsLong);
}

int main(int argc, char* argv[]) {
	int gridRes = 10;
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
	
	const char* inputFile = "data/input-observedhotspots.txt";
	const char* copyFile = "output/input-observedhotspots.txt";
	const char* limitFile = "output/limits.txt";
	const char* abcdDistFile = "output/abcdspaceprob.txt";
	const char* possibleHotspotsFile = "output/possiblehotspots.txt";
	
	std::ifstream src(inputFile);
	std::ofstream dst(copyFile);
	dst << src.rdbuf();
	printf("Copied input file to \"%s\".\n\n", copyFile);
	
	ObservedHotspots observedHotspots(inputFile);
	observedHotspots.Iterate(PrintCoord, NULL);
	printf("\n");
	
	AbcdSpaceLimits limits(observedHotspots);
	limits.PrintToFile(limitFile);
	limits.PrintToFile(stdout);
	printf("\n");
	
	AbcdSpaceProbabilityDistribution abcdDist(observedHotspots, limits, 1, 5);
	abcdDist.PrintToFile(abcdDistFile);
	printf("\n");
	
	PossibleHotspotsDistribution possibleHotspots(observedHotspots, limits, gridRes, increment, interval);
	possibleHotspots.PrintToFile(possibleHotspotsFile);
	
	return EXIT_SUCCESS;
}