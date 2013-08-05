#include <cstdio>
#include "HotspotCoords.h"

HotspotCoords::HotspotCoords() : 
	moonLat(MissingCoord),
	moonLong(MissingCoord),
	marsLat(MissingCoord),
	marsLong(MissingCoord) {
}

Coord* HotspotCoords::GetCoordArray() {
	Coord* coordArray = new Coord[4];
	coordArray[0] = moonLat;
	coordArray[1] = moonLong;
	coordArray[2] = marsLat;
	coordArray[3] = marsLong;
	return coordArray;
}

const short* HotspotCoords::GetNumCoordsArray() {
	short* numCoordsArray = new short[4];
	numCoordsArray[0] = NumLats;
	numCoordsArray[1] = NumLongs;
	numCoordsArray[2] = NumLats;
	numCoordsArray[3] = NumLongs;
	return numCoordsArray;
}

std::string HotspotCoords::ToString() {
	char buff[1024];
	sprintf(buff, "%6d%6d%6d%6d",
			moonLat,
			moonLong,
			marsLat,
			marsLong);
	
	return buff;
}

bool HotspotCoords::Compare(HotspotCoords a, HotspotCoords b) {
	if(a.moonLat != b.moonLat)
		return a.moonLat < b.moonLat;
	if(a.moonLong != b.moonLong)
		return a.moonLong < b.moonLong;
	if(a.marsLat != b.marsLat)
		return a.marsLat < b.marsLat;
	return a.marsLong < b.marsLong;
}

bool HotspotCoords::operator==(HotspotCoords other) {
	return !Compare(*this, other) && !Compare(other, *this);
}

bool HotspotCoords::operator!=(HotspotCoords other) {
	return !(*this == other);
}