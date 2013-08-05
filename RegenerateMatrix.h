#ifndef __REGENERATE_MATRIX__
#define __REGENERATE_MATRIX__


#include <string>
#include <vector>
#include <set>
#include "HotspotCoords.h"
#include "HotspotCoordsWithProbability.h"

class RegenerateMatrix {
public:
	RegenerateMatrix(std::string filename);
	
	bool IsRequired(int index);
	void RegenerateProbabilities(std::vector<HotspotCoordsWithProbability> &hotspotsIn);
	
private:
	struct MatElem {
		int toInd;
		int fromInd;
		int multiplier;
	};
	
	std::vector<MatElem> matrix;
	std::vector<HotspotCoords> possibleHotspots;
	std::set<int> requiredIndices;
	int numPoints;
};


#endif
