#include <cstdio>
#include <cstdlib>
#include <string>
#include "Common.h"
#include "ObservedHotspots.h"
#include "AbcdSpaceLimits.h"
#include "AbcdSpaceProbabilityDistribution.h"

int main(int argc, char* argv[]) {
	int gridRes = 5;
	int increment = 1;
	
	if (argc != 3) {
		printf("Usage: ./CNmoonmarsCountPoints gridRes increment\n");
		return EXIT_FAILURE;
	}
	
	gridRes = atoi(argv[1]);
	increment = atoi(argv[2]);
	
	printf("===============================================================\n");
	
	std::string inputFile = "data/input-observedhotspots.txt";
	
	printf("Point count is based on observed hotspots in file:\n");
	printf("%s\n\n", inputFile.c_str());
	
	ObservedHotspots observedHotspots(inputFile);
	//observedHotspots.Iterate(PrintCoord, NULL);
	//printf("\n");
	
	AbcdSpaceLimits limits(observedHotspots);
	limits.PrintToFile(stdout);
	printf("\n");
	
	long int count = AbcdSpaceProbabilityDistribution::CalculateNumberOfAbcdPoints(limits, gridRes, increment);
	printf("Point count with gridRes = %d, increment = %d:  %ld\n", gridRes, increment, count);
	
	return EXIT_SUCCESS;
}
