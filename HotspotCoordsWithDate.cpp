#include <cstdio>
#include "HotspotCoordsWithDate.h"

void HotspotCoordsWithDate::Print() {
	printf("%5d, %5d, %5d, %5d, %5d, %5d\n",
		   month.value,
		   year,
		   moonLat,
		   moonLong,
		   marsLat,
		   marsLong);
}