#ifndef __POSSIBLE_HOTSPOTS_DISTRIBUTION__
#define __POSSIBLE_HOTSPOTS_DISTRIBUTION__


#include <string>
#include <vector>
#include "AbcdSpaceLimits.h"
#include "AbcdSpaceProbabilityDistribution.h"
#include "HotspotCoordsWithProbability.h"

class PossibleHotspotsDistribution {
public:
	PossibleHotspotsDistribution(std::vector<HotspotCoordsWithProbability>* points);
	PossibleHotspotsDistribution(AbcdSpaceLimits limits, bool nonremovable);
	PossibleHotspotsDistribution(AbcdSpaceLimits limits, AbcdSpaceProbabilityDistribution* abcdDistribution); 
	PossibleHotspotsDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimits limits, 
								 int gridRes, int increment, int interval, std::string directory="/dev/null", int startIndex=0, int endIndex=0);
	
	void PrintToFile(std::string filename, bool printProbs = true);
	Double GetTotalProbability(PossibleHotspotsDistribution points);
	
	static void ValidateIndexLimits(int startIndex, int endIndex);
	static void AdjustStartEndIndices(AbcdSpaceLimits limits, int &startIndex, int &endIndex);
	static bool IsPartial(int startIndex, int endIndex);
	static void Normalize(std::vector<HotspotCoordsWithProbability>* points);
		
private:
	PossibleHotspotsDistribution(int startIndex, int endIndex); 

	void CalculatePossibleHotspotCoords(AbcdSpaceLimits limits, bool nonremovable = false);
	void AccumulateProbabilities(AbcdSpaceProbabilityDistribution* abcdDistribution);
	void Normalize();
	
	void PrintStatusFile(char* buff, char* filename);
	
	void AdjustStartEndIndices();
	bool IsPartial();
	
	std::vector <HotspotCoordsWithProbability> possibleHotspots;
	int startIndex;
	int endIndex;
	int gridRes;
	int increment;
	int interval;
};


#endif
