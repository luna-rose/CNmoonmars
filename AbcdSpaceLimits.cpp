#include <limits>
#include <cstdlib>
#include "AbcdSpaceLimits.h"

AbcdSpaceLimits::AbcdSpaceLimits(ObservedHotspots observedHotspots) {
	for (int i = 0 ; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			limits[i][j] = std::numeric_limits<double>::infinity();
		}
	}
	
	observedHotspots.Iterate(AdjustLimitsSingleHotspot, &limits);
	
	PairwiseCombineLimits();
}

void AbcdSpaceLimits::PrintToFile(const char* filename){
	FILE* file = fopen(filename, "w");
	if(!file) {
		printf("Error: Could not open file for writing: \"%s\"\n", filename);
		exit(EXIT_FAILURE);
	}
	
	PrintToFile(file);
	
	fclose(file);
	
	printf("Printed limits to file: \"%s\".\n", filename);
}

void AbcdSpaceLimits::PrintToFile(FILE* file){
	char letter[4] = {'a','b','c','d'};
	
	for (int i = 0 ; i < 4; i++) {
		for (int j = i+1; j < 4; j++) {
			fprintf(file, "%c_%c_min = %.17g\n", letter[j], letter[i], 1-limits[i][j]);
			fprintf(file, "%c_%c_max = %.17g\n", letter[j], letter[i], limits[j][i]);
		}
	}
}

AbcdSpaceLimitsInt AbcdSpaceLimits::GenerateAbcdSpaceLimitsInt(int scale) {
	AbcdSpaceLimitsInt returnval;
	for (int i = 0 ; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			returnval.limits[i][j] = (int)(limits[i][j]*HotspotCoords::NumLats*HotspotCoords::NumLongs*scale + 0.5);
		}
	}
	return returnval;
}

bool AbcdSpaceLimits::CheckHotspot(HotspotCoords coord) {
	Coord* coordArray = coord.GetCoordArray();
	const short* numCoordsArray = HotspotCoords::GetNumCoordsArray();
	AbcdSpaceLimitsInt limitsInt = GenerateAbcdSpaceLimitsInt(1); 
	
	for (int i = 0 ; i < 4; i++) {
		if (coordArray[i] != HotspotCoords::MissingCoord) {
			for (int j = 0; j < 4; j++) {
				if (i!=j && coordArray[j] != HotspotCoords::MissingCoord) {
					double newLimit = (coordArray[i]+0.5)/numCoordsArray[i] - (coordArray[j]-0.5)/numCoordsArray[j];
					while (newLimit < 0) newLimit += 1.0;
					while (newLimit >= 1.0) newLimit -= 1.0;
					int LimitFactor = HotspotCoords::NumLats*HotspotCoords::NumLongs;
					int intNewLim = (int)(newLimit*LimitFactor + 0.5);
					if(intNewLim <= LimitFactor-limitsInt.limits[j][i]) {
						delete(coordArray);
						delete(numCoordsArray);
						return false;
					}
				}
			}
		}
	}
	
	delete(coordArray);
	delete(numCoordsArray);
	return true;
}

void AbcdSpaceLimits::AdjustLimitsSingleHotspot(HotspotCoordsWithDate coord, void* data) {
	double (*limits)[4] = (double(*)[4])data; 
	
	Coord* coordArray = coord.GetCoordArray();
	const short* numCoordsArray = HotspotCoords::GetNumCoordsArray();
	
	for (int i = 0 ; i < 4; i++) {
		if (coordArray[i] != HotspotCoords::MissingCoord) {
			for (int j = 0; j < 4; j++) {
				if (coordArray[j] != HotspotCoords::MissingCoord) {
					double newLimit = (coordArray[i]+0.5)/numCoordsArray[i] - (coordArray[j]-0.5)/numCoordsArray[j];
					while (newLimit < 0) newLimit += 1.0;
					while (newLimit >= 1.0) newLimit -= 1.0;
					if(newLimit < limits[i][j])
						limits[i][j] = newLimit;
				}
			}
		}
	}
	
	delete(coordArray);
	delete(numCoordsArray);
}

void AbcdSpaceLimits::PairwiseCombineLimits() {
	bool changed;
	int count=0;
	do {
		changed = false;
		for (int i = 0 ; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 4; k++) {
					double newLimit = limits[i][k] + limits[k][j];
					while (newLimit < 0) newLimit += 1.0;
					while (newLimit >= 1.0) newLimit -= 1.0;
					if(newLimit < limits[i][j]) {
						limits[i][j] = newLimit;
						changed = true;
					}
				}
			}
		}
		count ++;
	} while(changed);
	printf("Required %d iterations for pairwise combininig limits\n", count);
}