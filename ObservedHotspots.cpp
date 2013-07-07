#include <cstdlib>
#include "ObservedHotspots.h"

ObservedHotspots::ObservedHotspots(std::string filename) {
	FILE* file = fopen(filename.c_str(), "r");
	if(!file) {
		printf("Error: Could not open file for reading: \"%s\"\n", filename.c_str());
		exit(EXIT_FAILURE);
	}
	
	while (!feof(file)) {
		HotspotCoordsWithDate coords;
		
		char monthStr[4];
		if(!fscanf(file, "%3s", monthStr)) {
			printf("Error: Could not read month from file.\n");
			exit(EXIT_FAILURE);
		}
		coords.month.Set(monthStr);
		
		if(fscanf(file, "-") != 0) {
			printf("Error: Could not read '-' from file.\n");
			exit(EXIT_FAILURE);
		}
			
		if(!fscanf(file, "%4hd", &coords.year)) {
			printf("Error: Could not read year from file.\n");
			exit(EXIT_FAILURE);
		}
		
		coords.moonLat = ScanCoordinate(file);
		coords.moonLong = ScanCoordinate(file);
		coords.marsLat = ScanCoordinate(file);
		coords.marsLong = ScanCoordinate(file);
		
		if(fscanf(file, "\n") != 0) {
			printf("Error: Could not read newline from file.\n");
			exit(EXIT_FAILURE);
		}
				
		observedHotspots.push_back(coords);
	}
	
	fclose(file);
}

ObservedHotspots::~ObservedHotspots() {
}

void ObservedHotspots::Iterate(void (*function)(HotspotCoordsWithDate coord, void* data), void* data){
	std::vector<HotspotCoordsWithDate>::iterator it;
	for (it = observedHotspots.begin(); it != observedHotspots.end(); ++it) {
		function(*it, data);
	}
}

Coord ObservedHotspots::ScanCoordinate(FILE* file){
	Coord result;
	if(!fscanf(file, "%5hd", &result)){
		result = HotspotCoords::MissingCoord;
	}
	return result;
}
