#include <cstdio>
#include "RegenerateMatrix.h"

RegenerateMatrix::RegenerateMatrix(std::string filename)
{	
	FILE* file = fopen(filename.c_str(), "r");
	if(!file) {
		printf("Error: Could not open file for reading: \"%s\"\n", filename.c_str());
		exit(EXIT_FAILURE);
	}
	
	if(!fscanf(file, "NUMBER OF POINTS: %d\n\n", &numPoints)) {
		printf("Error: Could not read numPoints from file: \"%s\".\n", filename.c_str());
		exit(EXIT_FAILURE);
	}
	
	for(long int i=0; i<numPoints; i++){
		HotspotCoords newCoord;
		if(!fscanf(file, "%6hd", &(newCoord.moonLat))) {
			printf("Error: Could not read moonLat from file: \"%s\".\n", filename.c_str());
			exit(EXIT_FAILURE);
		}
		if(!fscanf(file, "%6hd", &(newCoord.moonLong))) {
			printf("Error: Could not read moonLong from file: \"%s\".\n", filename.c_str());
			exit(EXIT_FAILURE);
		}
		if(!fscanf(file, "%6hd", &(newCoord.marsLat))) {
			printf("Error: Could not read marsLat from file: \"%s\".\n", filename.c_str());
			exit(EXIT_FAILURE);
		}
		if(!fscanf(file, "%6hd", &(newCoord.marsLong))) {
			printf("Error: Could not read marsLong from file: \"%s\".\n", filename.c_str());
			exit(EXIT_FAILURE);
		}
		if(fscanf(file, "\n") != 0) {
			printf("Error: Could not read newline from file.\n");
			exit(EXIT_FAILURE);
		}
		possibleHotspots.push_back(newCoord);
	}
	
	if(fscanf(file, "\n") != 0) {
		printf("Error: Could not read newline from file.\n");
		exit(EXIT_FAILURE);
	}
	
	while (!feof(file)) {
		MatElem matElem;
		
		if(!fscanf(file, "%6d", &(matElem.toInd))) {
			printf("Error: Could not read toInd from file: \"%s\".\n", filename.c_str());
			exit(EXIT_FAILURE);
		}
		if(!fscanf(file, "%6d", &(matElem.fromInd))) {
			printf("Error: Could not read fromInd from file: \"%s\".\n", filename.c_str());
			exit(EXIT_FAILURE);
		}
		if(!fscanf(file, "%6d", &(matElem.multiplier))) {
			printf("Error: Could not read multiplier from file: \"%s\".\n", filename.c_str());
			exit(EXIT_FAILURE);
		}
		if(fscanf(file, "\n") != 0) {
			printf("Error: Could not read newline from file.\n");
			exit(EXIT_FAILURE);
		}
		
		requiredIndices.insert(matElem.fromInd);
		matrix.push_back(matElem);
	}
	
	fclose(file);
	
	printf("Need to compute %zu points to generate probabilties.\n", requiredIndices.size());
}

bool RegenerateMatrix::IsRequired(int index)
{
	return requiredIndices.find(index) != requiredIndices.end();
}

void RegenerateMatrix::RegenerateProbabilities(std::vector<HotspotCoordsWithProbability> &hotspotsIn)
{
	if (numPoints != (int)hotspotsIn.size()) {
		printf("Error: Possible hotspot count from matrix, %d, does not match computed possible hotspot count, %zu.\n",
			   numPoints, hotspotsIn.size());
		exit(EXIT_FAILURE);
	}
	
	for (int i=0; i<numPoints; i++) {
		if (possibleHotspots[i] != hotspotsIn[i]) {
			printf("Error: Coordinate %d from M file do not match calculated coordinates:\n",
				i+1);
			printf("From M file: %s.\n", possibleHotspots[i].ToString().c_str());
			printf("Calculated : %s.\n",  ((HotspotCoords)hotspotsIn[i]).ToString().c_str());
			exit(EXIT_FAILURE);
		}
	}
	
	printf("Regenerating all %d points from %zu calculated points.\n", numPoints,requiredIndices.size());
	
	std::vector<Double> savedProbs;
	for (std::vector<HotspotCoordsWithProbability>::iterator hotspot = hotspotsIn.begin(); hotspot<hotspotsIn.end(); hotspot++) {
		savedProbs.push_back(hotspot->prob);
		hotspot->prob = 0;
	}
	
	for (std::vector<MatElem>::iterator matElem = matrix.begin(); matElem<matrix.end(); matElem++) {
		int toInd = matElem->toInd;
		int fromInd = matElem->fromInd;
		int multiplier = matElem->multiplier;
		
		hotspotsIn[toInd].prob += multiplier*savedProbs[fromInd];
	}
}