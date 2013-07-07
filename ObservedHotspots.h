#ifndef __OBSERVED_HOTSPOTS__
#define __OBSERVED_HOTSPOTS__


#include <string>
#include <vector>
#include <cstdio>
#include "HotspotCoordsWithDate.h"

class ObservedHotspots {
public:
	ObservedHotspots(std::string filename);
	~ObservedHotspots();
	
	void Iterate(void (*function)(HotspotCoordsWithDate coord, void* data), void* data);
	
private:
	Coord ScanCoordinate(FILE* file);
	
	std::vector <HotspotCoordsWithDate> observedHotspots;
};


#endif
