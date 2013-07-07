#include <cstdio>
#include "Common.h"

void PrintCoord(HotspotCoordsWithDate coord, void* data) {
	printf("%5d, %5d, %5d, %5d, %5d, %5d\n", 
		   coord.month.value, 
		   coord.year,
		   coord.moonLat,
		   coord.moonLong,
		   coord.marsLat,
		   coord.marsLong);
}