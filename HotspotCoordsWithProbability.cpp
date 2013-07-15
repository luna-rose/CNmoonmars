#include <cstdio>
#include "HotspotCoordsWithProbability.h"

std::string HotspotCoordsWithProbability::ToString() {
	char buff[1024];
	sprintf(buff, "%6d%6d%6d%6d%46.36Le",
			moonLat,
			moonLong,
			marsLat,
			marsLong,
			prob);
	
	return buff;
}
