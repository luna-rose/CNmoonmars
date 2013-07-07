#ifndef __ABCD_SPACE_PROBABILITY_DISTRIBUTION__
#define __ABCD_SPACE_PROBABILITY_DISTRIBUTION__


#include <vector>
#include "ObservedHotspots.h"
#include "AbcdSpaceLimits.h"

class AbcdSpaceProbabilityDistribution {
public:
	AbcdSpaceProbabilityDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimits limits, int gridRes, int increment, bool normalize = true);
	AbcdSpaceProbabilityDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimitsInt limits, int gridRes, int increment, bool normalize = true);
	~AbcdSpaceProbabilityDistribution();
	
	void PrintToFile(const char* filename);
	
	double CalculateHotspotProbability(const HotspotCoords coord, double prob = 0);
	
	static long int CalculateNumberOfAbcdPoints(AbcdSpaceLimits limits, int gridRes, int increment);
	
	long int GetNumPoints();

private:
	struct AbcdSpacePoint {
		double ba;
		double ca;
		double da;
		double prob;
		
		AbcdSpacePoint(){}
		
		AbcdSpacePoint(double inBa, double inCa, double inDa, double inProb){
			ba = inBa;
			ca = inCa;
			da = inDa;
			prob = inProb;
		}
	};
	
	void CalculateProbabilityDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimits limits, int gridRes, int increment, bool normalize = true);
	void CalculateProbabilityDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimitsInt limits, int gridRes, int increment, bool normalize = true);
	void static CalculateProbSingleHotspot(HotspotCoordsWithDate coord, void* data);
	
	static long int CalculateNumberOfAbcdPoints(AbcdSpaceLimitsInt limsInt, int gridRes, int increment, std::vector<long int>* starts = NULL);
	
	AbcdSpacePoint* probPoints;
	long int numProbPoints;
};


#endif

