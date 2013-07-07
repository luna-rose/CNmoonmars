#include <cstdlib>
#include "PossibleHotspotsDistribution.h"

PossibleHotspotsDistribution::PossibleHotspotsDistribution(AbcdSpaceLimits limits, AbcdSpaceProbabilityDistribution* abcdDistribution) {
	CalculatePossibleHotspotCoords(limits);
	AccumulateProbabilities(abcdDistribution);
	printf("Probability distribution contains %ld points.\n", abcdDistribution->GetNumPoints());
}

PossibleHotspotsDistribution::PossibleHotspotsDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimits limits, int gridRes, int increment, int interval) {
	CalculatePossibleHotspotCoords(limits);
	
	long int preCalcNumPoints = AbcdSpaceProbabilityDistribution::CalculateNumberOfAbcdPoints(limits, gridRes, increment);
	printf("Precomputed number of abcd space points: %ld.\n\n", preCalcNumPoints);
	
	int LimitCount = HotspotCoords::NumLats*HotspotCoords::NumLongs*gridRes;
	
	AbcdSpaceLimitsInt abcdSpaceLimits = limits.GenerateAbcdSpaceLimitsInt(gridRes);
	int numChunks = (abcdSpaceLimits.limits[1][0] + abcdSpaceLimits.limits[0][1] - LimitCount - 1)/(increment*interval);
	
	AbcdSpaceLimitsInt partialSpaceLimits = abcdSpaceLimits;
	partialSpaceLimits.limits[1][0] = LimitCount - partialSpaceLimits.limits[0][1] + increment*interval + 1;
	
	int chunkCount = 0;
	long int pointCount = 0;
	while (LimitCount - partialSpaceLimits.limits[0][1] + 1 < abcdSpaceLimits.limits[1][0]) {
		if(partialSpaceLimits.limits[1][0] > abcdSpaceLimits.limits[1][0])
			partialSpaceLimits.limits[1][0] = abcdSpaceLimits.limits[1][0];
		
		AbcdSpaceProbabilityDistribution* abcdDistribution;
		abcdDistribution = new AbcdSpaceProbabilityDistribution(observedHotspots, partialSpaceLimits, gridRes, increment, false);
		AccumulateProbabilities(abcdDistribution);	
		
		pointCount += abcdDistribution->GetNumPoints();
		chunkCount ++;
		printf("Chunk %4d of %4d,     Chunk points: %9ld,     Total points: %12ld\n", 
			   chunkCount, numChunks, abcdDistribution->GetNumPoints(), pointCount);
		
		delete(abcdDistribution);
		
		partialSpaceLimits.limits[1][0]+=increment*interval;
		partialSpaceLimits.limits[0][1]-=increment*interval;
	}
	printf("\nTotal points in the probability distribution: %ld.\n", pointCount);
	
	if (chunkCount != numChunks) {
		printf("!!! ERROR: PRECOMPUTED CHUNK COUNT, %d, DOES NOT MATCH ACTUAL CHUNK COUNT, %d !!!\n\n", 
			   numChunks, chunkCount);
	}
	if (pointCount != preCalcNumPoints) {
		printf("!!! ERROR: PRECOMPUTED POINT COUNT, %ld, DOES NOT MATCH ACTUAL POINT COUNT, %ld !!!\n\n", 
			   preCalcNumPoints, pointCount);
	}
	
	Normalize();
}

void PossibleHotspotsDistribution::CalculatePossibleHotspotCoords(AbcdSpaceLimits limits) {
	Coord minLat = HotspotCoords::MinLat;
	Coord maxLat = HotspotCoords::MaxLat;
	Coord minLong = HotspotCoords::MinLong;
	Coord maxLong = HotspotCoords::MaxLong;
	
	for (Coord moonLat = minLat; moonLat <= maxLat; moonLat ++) {
		HotspotCoordsWithProbability coords;
		coords.moonLat = moonLat;
		if (limits.CheckHotspot(coords)) {
			
			for (Coord moonLong = minLong; moonLong <= maxLong; moonLong ++) {
				coords.moonLong = moonLong;
				if (limits.CheckHotspot(coords)) {
					
					for (Coord marsLat = minLat; marsLat <= maxLat; marsLat ++) {
						coords.marsLat = marsLat;
						if (limits.CheckHotspot(coords)) {
							
							for (Coord marsLong = minLong; marsLong <= maxLong; marsLong ++) {
								coords.marsLong = marsLong;
								if (limits.CheckHotspot(coords)) {
									coords.prob = 0;
									possibleHotspots.push_back(coords);
								}
							}
							coords.marsLong = HotspotCoords::MissingCoord;
						}
					}
					coords.marsLat = HotspotCoords::MissingCoord;					
				}
			}
			coords.moonLong = HotspotCoords::MissingCoord;
		}
	}
	
	printf("Found %zu possible hotspots.\n", possibleHotspots.size());
}

void PossibleHotspotsDistribution::AccumulateProbabilities(AbcdSpaceProbabilityDistribution* abcdDistribution) {
	#ifdef using_parallel
	#pragma omp parallel for
	#endif
	for (int i=0; i<(int)possibleHotspots.size(); i++) {
		possibleHotspots[i].prob = abcdDistribution->CalculateHotspotProbability(possibleHotspots[i], possibleHotspots[i].prob);
	}
}

void PossibleHotspotsDistribution::PrintToFile(const char* filename){
	FILE* file = fopen(filename, "w");
	if(!file) {
		printf("Error: Could not open file for writing: \"%s\"\n", filename);
		exit(EXIT_FAILURE);
	}
	
	for(std::vector<HotspotCoordsWithProbability>::iterator it = possibleHotspots.begin(); it<possibleHotspots.end(); it++){
		HotspotCoordsWithProbability coords = *it;
		fprintf(file, "%6d%6d%6d%6d%28.18le\n", coords.moonLat, coords.moonLong, coords.marsLat, coords.marsLong, coords.prob);
	}
	
	fclose(file);
	
	printf("Printed possible hotspots with probabilities to file: \"%s\".\n", filename);
}

void PossibleHotspotsDistribution::Normalize() {
	double sumProb = 0;
	for(std::vector<HotspotCoordsWithProbability>::iterator coord=possibleHotspots.begin(); coord<possibleHotspots.end(); coord++)
		sumProb += coord->prob;
	for(std::vector<HotspotCoordsWithProbability>::iterator coord=possibleHotspots.begin(); coord<possibleHotspots.end(); coord++)
		coord->prob /= sumProb;	
}
