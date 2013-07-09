#include <cstdlib>
#include <cmath>
#include "PossibleHotspotsDistribution.h"

PossibleHotspotsDistribution::PossibleHotspotsDistribution(int inStartIndex, int inEndIndex) :
	startIndex(inStartIndex),
	endIndex(inEndIndex)
{
	ValidateIndexLimits(startIndex, endIndex);
}

PossibleHotspotsDistribution::PossibleHotspotsDistribution(AbcdSpaceLimits limits, AbcdSpaceProbabilityDistribution* abcdDistribution) :
	startIndex(0),
	endIndex(0)
{
	ValidateIndexLimits(startIndex, endIndex);
	CalculatePossibleHotspotCoords(limits);
	AccumulateProbabilities(abcdDistribution);
	printf("Probability distribution contains %ld points.\n", abcdDistribution->GetNumPoints());
}

PossibleHotspotsDistribution::PossibleHotspotsDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimits limits,
							int inGridRes, int inIncrement, int inInterval, std::string directory, int inStartIndex, int inEndIndex) :
	startIndex(inStartIndex),
	endIndex(inEndIndex),
	gridRes(inGridRes),
	increment(inIncrement),
	interval(inInterval)
{
	ValidateIndexLimits(startIndex, endIndex);
	CalculatePossibleHotspotCoords(limits);
	
	long int preCalcNumPoints = AbcdSpaceProbabilityDistribution::CalculateNumberOfAbcdPoints(limits, gridRes, increment);
	printf("Precomputed number of abcd space points: %ld.\n\n", preCalcNumPoints);
	
	int LimitCount = HotspotCoords::NumLats*HotspotCoords::NumLongs*gridRes;
	
	AbcdSpaceLimitsInt abcdSpaceLimits = limits.GenerateAbcdSpaceLimitsInt(gridRes);
	
	int minBa = LimitCount - abcdSpaceLimits.limits[0][1];
	int maxBa = abcdSpaceLimits.limits[1][0];
	int numChunks = ceil((Double)((maxBa - minBa - 1)/increment)/interval);
	
	AbcdSpaceLimitsInt partialSpaceLimits = abcdSpaceLimits;
	partialSpaceLimits.limits[1][0] = LimitCount - partialSpaceLimits.limits[0][1] + increment*interval + 1;
	
	fflush(stdout);
	
	int chunkCount = 0;
	long int pointCount = 0;
	while (LimitCount - partialSpaceLimits.limits[0][1] + increment < maxBa) {
		if(partialSpaceLimits.limits[1][0] > maxBa)
			partialSpaceLimits.limits[1][0] = maxBa;
		
		AbcdSpaceProbabilityDistribution* abcdDistribution;
		abcdDistribution = new AbcdSpaceProbabilityDistribution(observedHotspots, partialSpaceLimits, gridRes, increment, false);
		AccumulateProbabilities(abcdDistribution);	
		
		pointCount += abcdDistribution->GetNumPoints();
		chunkCount ++;
		
		char buff[1024];
		sprintf(buff, "Chunk %4d of %4d,     Chunk points: %9ld,     Total points: %12ld\n",
				chunkCount, numChunks, abcdDistribution->GetNumPoints(), pointCount);
		printf("%s",buff);
		fflush(stdout);
		
		if (directory != "/dev/null") {
			char filename[1024];
			sprintf(filename, "%schunk%06d.txt", directory.c_str(), chunkCount);
			PrintStatusFile(buff, filename);
		}
		
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
	
	if (!IsPartial()) {
		Normalize();
	}
}

void PossibleHotspotsDistribution::PrintStatusFile(char* buff, char* filename) {
	FILE* file = fopen(filename, "w");
	if(!file) {
		printf("Error: Could not open file for writing: \"%s\"\n", filename);
		exit(EXIT_FAILURE);
	}
	
	fprintf(file, "%s", buff);
	
	fclose(file);
}

bool PossibleHotspotsDistribution::IsPartial(int startIndex, int endIndex) {
	return !(startIndex == 0 && endIndex == 0);
}

bool PossibleHotspotsDistribution::IsPartial() {
	return IsPartial(startIndex, endIndex);
}

void PossibleHotspotsDistribution::AdjustStartEndIndices() {
	if (startIndex > (int)possibleHotspots.size())
		startIndex = possibleHotspots.size();
	if (endIndex > (int)possibleHotspots.size())
		endIndex = possibleHotspots.size();
	
	if (startIndex == 1 && endIndex == (int)possibleHotspots.size()) {
		startIndex = 0;
		endIndex = 0;
	}
}

void PossibleHotspotsDistribution::AdjustStartEndIndices(AbcdSpaceLimits limits, int &startIndex, int &endIndex)
{
	PossibleHotspotsDistribution possibleHotspots(startIndex, endIndex);
	possibleHotspots.CalculatePossibleHotspotCoords(limits);
	startIndex = possibleHotspots.startIndex;
	endIndex = possibleHotspots.endIndex;
}

void PossibleHotspotsDistribution::ValidateIndexLimits(int startIndex, int endIndex) {
	if(!IsPartial(startIndex, endIndex))
		return;
	
	if(startIndex >= 1 && endIndex >= startIndex)
		return;
	
	printf("Error: Invalid start & end indices: start = %d, end = %d.\n", startIndex, endIndex);
	exit(EXIT_FAILURE);
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
	
	AdjustStartEndIndices();
}

void PossibleHotspotsDistribution::AccumulateProbabilities(AbcdSpaceProbabilityDistribution* abcdDistribution) {
	int start = 0;
	int end = possibleHotspots.size() - 1;
	
	if (IsPartial()) {
		start = startIndex - 1;
		end = endIndex - 1;
	}
	
	#ifdef using_parallel
	#pragma omp parallel for
	#endif
	for (int i=start; i<=end; i++) {
		possibleHotspots[i].prob = abcdDistribution->CalculateHotspotProbability(possibleHotspots[i], possibleHotspots[i].prob);
	}
}

void PossibleHotspotsDistribution::PrintToFile(std::string filename){
	FILE* file = fopen(filename.c_str(), "w");
	if(!file) {
		printf("Error: Could not open file for writing: \"%s\"\n", filename.c_str());
		exit(EXIT_FAILURE);
	}
	
	if(IsPartial()) {
		fprintf(file, "!! THIS IS A PARTIAL FILE !!\n");
		fprintf(file, "START INDEX = %4d\n", startIndex);
		fprintf(file, "END   INDEX = %4d\n", endIndex);
		fprintf(file, "GRID  RES   = %4d\n", gridRes);
		fprintf(file, "INCREMENT   = %4d\n", increment);
		fprintf(file, "INTERVAL    = %4d\n\n", interval);
		fprintf(file, "PROBABILITIES ARE NOT NORMALIZED\n\n");
	}
	
	for(std::vector<HotspotCoordsWithProbability>::iterator it = possibleHotspots.begin(); it<possibleHotspots.end(); it++){
		HotspotCoordsWithProbability coords = *it;
		fprintf(file, "%6d%6d%6d%6d%46.36Le\n", coords.moonLat, coords.moonLong, coords.marsLat, coords.marsLong, coords.prob);
	}
	
	fclose(file);
	
	printf("Printed possible hotspots with probabilities to file: \"%s\".\n", filename.c_str());
	if(IsPartial()) {
		printf("Generated partial file from index %d to %d.\n", startIndex, endIndex);
	}
}

void PossibleHotspotsDistribution::Normalize() {
	Double sumProb = 0;
	for(std::vector<HotspotCoordsWithProbability>::iterator coord=possibleHotspots.begin(); coord<possibleHotspots.end(); coord++)
		sumProb += coord->prob;
	for(std::vector<HotspotCoordsWithProbability>::iterator coord=possibleHotspots.begin(); coord<possibleHotspots.end(); coord++)
		coord->prob /= sumProb;	
}
