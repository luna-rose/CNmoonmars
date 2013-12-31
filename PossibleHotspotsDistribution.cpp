#include <algorithm>
#include <cstdlib>
#include <cmath>
#include "PossibleHotspotsDistribution.h"

PossibleHotspotsDistribution::PossibleHotspotsDistribution(std::vector<HotspotCoordsWithProbability>* points) :
startIndex(0),
endIndex(0)
{
	ValidateIndexLimits(startIndex, endIndex);
	possibleHotspots = *points;
}

PossibleHotspotsDistribution::PossibleHotspotsDistribution(int inStartIndex, int inEndIndex) :
	startIndex(inStartIndex),
	endIndex(inEndIndex)
{
	ValidateIndexLimits(startIndex, endIndex);
}

PossibleHotspotsDistribution::PossibleHotspotsDistribution(AbcdSpaceLimits limits, bool nonremovable) :
startIndex(0),
endIndex(0)
{
	ValidateIndexLimits(startIndex, endIndex);
	CalculatePossibleHotspotCoords(limits, nonremovable);
}

PossibleHotspotsDistribution::PossibleHotspotsDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimits limits, RegenerateMatrix* regenMat,
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
		AccumulateProbabilities(abcdDistribution, regenMat);
		
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
		if (regenMat != NULL) {
			regenMat->RegenerateProbabilities(possibleHotspots);
		}
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

void PossibleHotspotsDistribution::CalculatePossibleHotspotCoords(AbcdSpaceLimits limits, bool nonremovable) {
	Coord minLat = HotspotCoords::MinLat;
	Coord maxLat = HotspotCoords::MaxLat;
	Coord minLong = HotspotCoords::MinLong;
	Coord maxLong = HotspotCoords::MaxLong;
	
	for (Coord moonLat = minLat; moonLat <= maxLat; moonLat ++) {
		HotspotCoordsWithProbability coords;
		coords.moonLat = moonLat;
		if (limits.CheckHotspot(coords, nonremovable)) {
			
			for (Coord moonLong = minLong; moonLong <= maxLong; moonLong ++) {
				coords.moonLong = moonLong;
				if (limits.CheckHotspot(coords, nonremovable)) {
					
					for (Coord marsLat = minLat; marsLat <= maxLat; marsLat ++) {
						coords.marsLat = marsLat;
						if (limits.CheckHotspot(coords, nonremovable)) {
							
							for (Coord marsLong = minLong; marsLong <= maxLong; marsLong ++) {
								coords.marsLong = marsLong;
								if (limits.CheckHotspot(coords, nonremovable)) {
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

void PossibleHotspotsDistribution::AccumulateProbabilities(AbcdSpaceProbabilityDistribution* abcdDistribution, RegenerateMatrix* regenMat) {
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
		if(regenMat == NULL || regenMat->IsRequired(i))
			possibleHotspots[i].prob = abcdDistribution->CalculateHotspotProbability(possibleHotspots[i], possibleHotspots[i].prob);
	}
}

void PossibleHotspotsDistribution::PrintToFile(std::string filename, bool printProbs){
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
		std::string coordStr;
		if(printProbs)
			coordStr = coords.ToString();
		else
			coordStr = ((HotspotCoords)coords).ToString();
		fprintf(file, "%s\n", coordStr.c_str());
	}
	
	fclose(file);
	
	if(printProbs)
		printf("Printed hotspots with probabilities to file: \"%s\".\n", filename.c_str());
	else {
		printf("Printed hotspots to file: \"%s\".\n", filename.c_str());
	}

	if(IsPartial()) {
		printf("Generated partial file from index %d to %d.\n", startIndex, endIndex);
	}
}

void PossibleHotspotsDistribution::Normalize() {
	Normalize(&possibleHotspots);	
}

void PossibleHotspotsDistribution::Normalize(std::vector<HotspotCoordsWithProbability>* points) {
	Double sumProb = 0;
	for(std::vector<HotspotCoordsWithProbability>::iterator coord=points->begin(); coord<points->end(); coord++)
		sumProb += coord->prob;
	for(std::vector<HotspotCoordsWithProbability>::iterator coord=points->begin(); coord<points->end(); coord++)
		coord->prob /= sumProb;	
}

Double PossibleHotspotsDistribution::GetTotalProbability(PossibleHotspotsDistribution points) {
	std::vector<HotspotCoordsWithProbability> allPoints = possibleHotspots;
	std::vector<HotspotCoordsWithProbability> selectedPoints = points.possibleHotspots;
	
	std::sort(allPoints.begin(), allPoints.end(), HotspotCoords::Compare);
	std::sort(selectedPoints.begin(), selectedPoints.end(), HotspotCoords::Compare);
	
	std::vector<HotspotCoordsWithProbability>::iterator itAll = allPoints.begin();
	std::vector<HotspotCoordsWithProbability>::iterator itSel = selectedPoints.begin();
	
	int matchedPoints = 0;
	Double totalProb = 0;
	while (true) {
		if(itSel == selectedPoints.end()) {
			break;
		}
		
		if(itAll == allPoints.end()) {
			printf("Error: Reached end of point list with selected points remaining.\n");
			printf("Next selected point is: %s.\n", ((HotspotCoords)(*itSel)).ToString().c_str());
			exit(EXIT_FAILURE);
		}
		
		// *itAll < *itSel
		if(HotspotCoords::Compare(*itAll, *itSel)) {
			itAll++;
			continue;
		}
		   
		// *itAll > *itSel
		if(HotspotCoords::Compare(*itSel, *itAll)) {
			printf("Error: Could not match selected point: %s.\n", ((HotspotCoords)(*itSel)).ToString().c_str());
			exit(EXIT_FAILURE);
		}
		
		// points are equal
		totalProb += itAll->prob;
		matchedPoints++;
		itAll++;
		itSel++;
	}
	if (matchedPoints != (int)selectedPoints.size()) {
		printf("Error: Matched point count, %d, does not match total number of selected points, %zu.\n", 
			   matchedPoints, selectedPoints.size());
		exit(EXIT_FAILURE);
	}
	
	return totalProb;
}
