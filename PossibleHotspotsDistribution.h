#ifndef __POSSIBLE_HOTSPOTS_DISTRIBUTION__
#define __POSSIBLE_HOTSPOTS_DISTRIBUTION__


#include <string>
#include <vector>
#include "AbcdSpaceLimits.h"
#include "AbcdSpaceProbabilityDistribution.h"
#include "HotspotCoordsWithProbability.h"

class PossibleHotspotsDistribution {
public:
	PossibleHotspotsDistribution(AbcdSpaceLimits limits, AbcdSpaceProbabilityDistribution* abcdDistribution); 
	PossibleHotspotsDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimits limits, 
								 int gridRes, int increment, int interval, std::string directory="/dev/null", int startIndex=0, int endIndex=0);
	
	void PrintToFile(std::string filename);
	
	static void ValidateIndexLimits(int startIndex, int endIndex);
	
private:
	void CalculatePossibleHotspotCoords(AbcdSpaceLimits limits);
	void AccumulateProbabilities(AbcdSpaceProbabilityDistribution* abcdDistribution);
	void Normalize();
	
	void PrintStatusFile(char* buff, char* filename);
	
	std::vector <HotspotCoordsWithProbability> possibleHotspots;
	int startIndex;
	int endIndex;
};


#endif
