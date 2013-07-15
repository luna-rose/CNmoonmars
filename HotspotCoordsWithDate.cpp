#include <cstdio>
#include "HotspotCoordsWithDate.h"

std::string HotspotCoordsWithDate::ToString() {
	char buff[1024];
	sprintf(buff, "%5d, %5d, %5d, %5d, %5d, %5d",
			month.value,
			year,
			moonLat,
			moonLong,
			marsLat,
			marsLong);
	
	return buff;
}
