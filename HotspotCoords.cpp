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
