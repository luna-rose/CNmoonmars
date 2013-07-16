#ifndef __HOTSPOT_COORDS__
#define __HOTSPOT_COORDS__


#include <climits>
#include <string>

typedef short Coord;

class HotspotCoords {
public:
	HotspotCoords();
	
	std::string ToString();
	
	Coord* GetCoordArray();
	static const short* GetNumCoordsArray();
	
	static bool Compare(HotspotCoords a, HotspotCoords b);
	
	static const Coord MissingCoord = SHRT_MAX;
	
	static const short MinLat = -84;
	static const short MaxLat = 84;
	static const short MinLong = -179;
	static const short MaxLong = 179;
	
	static const short NumLats = MaxLat - MinLat + 1;
	static const short NumLongs = MaxLong - MinLong + 1;
	
	Coord moonLat;
	Coord moonLong;
	Coord marsLat;
	Coord marsLong;
};


#endif
