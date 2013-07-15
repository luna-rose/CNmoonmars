#ifndef __HOTSPOT_COORDS_WITH_DATE__
#define __HOTSPOT_COORDS_WITH_DATE__


#include <string>
#include "HotspotCoords.h"
#include "Month.h"

typedef short Year;

class HotspotCoordsWithDate : public HotspotCoords {
public:
	std::string ToString();
	
	Month month;
	Year year;
};


#endif
