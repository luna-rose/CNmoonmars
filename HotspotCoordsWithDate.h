#ifndef __HOTSPOT_COORDS_WITH_DATE__
#define __HOTSPOT_COORDS_WITH_DATE__


#include "HotspotCoords.h"
#include "Month.h"

typedef short Year;

class HotspotCoordsWithDate : public HotspotCoords {
public:
	Month month;
	Year year;
};


#endif