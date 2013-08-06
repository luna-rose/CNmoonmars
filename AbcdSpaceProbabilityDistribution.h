#ifndef __ABCD_SPACE_PROBABILITY_DISTRIBUTION__
#define __ABCD_SPACE_PROBABILITY_DISTRIBUTION__


#include <string>
#include <vector>
#include "Common.h"
#include "ObservedHotspots.h"
#include "AbcdSpaceLimits.h"

class AbcdSpaceProbabilityDistribution {
public:
	AbcdSpaceProbabilityDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimits limits, int gridRes, int increment, bool normalize = true);
	AbcdSpaceProbabilityDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimitsInt limits, int gridRes, int increment, bool normalize = true);
	~AbcdSpaceProbabilityDistribution();
	
	void PrintToFile(std::string filename);
	
	Double CalculateHotspotProbability(const HotspotCoords coord, Double prob = 0);
	
	static long int CalculateNumberOfAbcdPoints(AbcdSpaceLimits limits, int gridRes, int increment);
	
	long int GetNumPoints();

private:
	struct AbcdSpacePoint {
		int ba;
		int ca;
		int da;
		Double prob;
		
		AbcdSpacePoint(){}
		
		AbcdSpacePoint(int inBa, int inCa, int inDa, Double inProb){
			ba = inBa;
			ca = inCa;
			da = inDa;
			prob = inProb;
		}
	};
	
	struct AbcdSpacePointWithLimitCount {
		AbcdSpacePoint* point;
		int LimitCount;
	};
	
	void Normalize();
	void ComputeProbabilities(ObservedHotspots observedHotspots);
	
	void CalculateProbabilityDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimits limits, int gridRes, int increment, bool normalize = true);
	void CalculateProbabilityDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimitsInt limits, int gridRes, int increment, bool normalize = true);
	void static CalculateProbSingleHotspot(HotspotCoordsWithDate coord, void* data);
	
	static long int CalculateNumberOfAbcdPoints(AbcdSpaceLimitsInt limsInt, int gridRes, int increment, std::vector<long int>* starts = NULL);
	
	AbcdSpacePoint* probPoints;
	long int numProbPoints;
	int LimitCount;
};


#endif
