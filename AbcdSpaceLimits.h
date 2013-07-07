#ifndef __ABCD_SPACE_LIMITS__
#define __ABCD_SPACE_LIMITS__


#include <cstdio>
#include <string>
#include "Common.h"
#include "ObservedHotspots.h"
#include "AbcdSpaceLimitsInt.h"

class AbcdSpaceLimits {
public:
	AbcdSpaceLimits(ObservedHotspots observedHotspots);
	
	void PrintToFile(std::string filename);
	void PrintToFile(FILE* file);
	
	AbcdSpaceLimitsInt GenerateAbcdSpaceLimitsInt(int scale);
	bool CheckHotspot(HotspotCoords coords);
	
private:
	void static AdjustLimitsSingleHotspot(HotspotCoordsWithDate coord, void* data);
	void PairwiseCombineLimits();
	
	// limits indicates how much higher 
	// the first index is above the second one
	// i.e. b_a_max is limits[1][0]
	//      b_a_min is -limits[0][1]
	Double limits[4][4];
};
					

#endif
