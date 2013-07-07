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
	PossibleHotspotsDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimits limits, int gridRes, int increment, int interval, std::string directory);
	
	void PrintToFile(std::string filename);
	
private:
	void CalculatePossibleHotspotCoords(AbcdSpaceLimits limits);
	void AccumulateProbabilities(AbcdSpaceProbabilityDistribution* abcdDistribution);
	void Normalize();
	
	void PrintStatusFile(char* buff, char* filename);
	
	std::vector <HotspotCoordsWithProbability> possibleHotspots;
};


#endif
