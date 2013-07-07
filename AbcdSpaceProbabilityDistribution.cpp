#include <cstdio>
#include <cstdlib>
#include "AbcdSpaceProbabilityDistribution.h"

AbcdSpaceProbabilityDistribution::AbcdSpaceProbabilityDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimits limits, 
																   int gridRes, int increment, bool normalize){
	CalculateProbabilityDistribution(observedHotspots, limits, gridRes, increment, normalize);
}

AbcdSpaceProbabilityDistribution::AbcdSpaceProbabilityDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimitsInt limits, 
																   int gridRes, int increment, bool normalize){
	CalculateProbabilityDistribution(observedHotspots, limits, gridRes, increment, normalize);
}

AbcdSpaceProbabilityDistribution::~AbcdSpaceProbabilityDistribution() {
	delete[] probPoints;
}

long int AbcdSpaceProbabilityDistribution::GetNumPoints() {
	return numProbPoints;
}

void AbcdSpaceProbabilityDistribution::PrintToFile(const char* filename){
	FILE* file = fopen(filename, "w");
	if(!file) {
		printf("Error: Could not open file for writing: \"%s\"\n", filename);
		exit(EXIT_FAILURE);
	}
	
	for(long int i=0; i<numProbPoints; i++){
		AbcdSpacePoint point = probPoints[i];
		fprintf(file, "%44.36Lf %44.36Lf %44.36Lf %47.36Le\n", point.ba, point.ca, point.da, point.prob);
	}
	
	fclose(file);
	
	printf("Probability distribution contains %ld points.\n", numProbPoints);
	printf("Printed probability distribution to file: \"%s\".\n", filename);
}

Double AbcdSpaceProbabilityDistribution::CalculateHotspotProbability(const HotspotCoords coord, Double prob) {
	if (coord.moonLat == HotspotCoords::MissingCoord ||
		coord.moonLong == HotspotCoords::MissingCoord ||
		coord.marsLat == HotspotCoords::MissingCoord ||
		coord.marsLong == HotspotCoords::MissingCoord) {
		printf("Error: Coordinates are missing: (%d, %d, %d, %d)\n", 
			   coord.moonLat, coord.moonLong, coord.marsLat, coord.marsLong);
		exit(EXIT_FAILURE);
	}
	
	for(long int i=0; i<numProbPoints; i++){
		AbcdSpacePoint point = probPoints[i];
		
		Double xmin = -0.5/HotspotCoords::NumLats;
		Double xmax = 0.5/HotspotCoords::NumLats;
		
		Double a = ((Double)coord.moonLat)/HotspotCoords::NumLats;
		Double b = a + point.ba;
		Double c = a + point.ca;
		Double d = a + point.da;
		
		Double bxmin = ((Double)coord.moonLong-0.5)/HotspotCoords::NumLongs - b;
		Double bxmax = ((Double)coord.moonLong+0.5)/HotspotCoords::NumLongs - b;
		while (bxmin > 0.5) bxmin -= 1.0;
		while (bxmax > 0.5) bxmax -= 1.0;
		while (bxmin < -0.5) bxmin += 1.0;
		while (bxmax < -0.5) bxmax += 1.0;
		if(bxmin > xmin) xmin = bxmin;
		if(bxmax < xmax) xmax = bxmax;
		
		Double cxmin = ((Double)coord.marsLat-0.5)/HotspotCoords::NumLats - c;
		Double cxmax = ((Double)coord.marsLat+0.5)/HotspotCoords::NumLats - c;
		while (cxmin > 0.5) cxmin -= 1.0;
		while (cxmax > 0.5) cxmax -= 1.0;
		while (cxmin < -0.5) cxmin += 1.0;
		while (cxmax < -0.5) cxmax += 1.0;
		if(cxmin > xmin) xmin = cxmin;
		if(cxmax < xmax) xmax = cxmax;
		
		Double dxmin = ((Double)coord.marsLong-0.5)/HotspotCoords::NumLongs - d;
		Double dxmax = ((Double)coord.marsLong+0.5)/HotspotCoords::NumLongs - d;
		while (dxmin > 0.5) dxmin -= 1.0;
		while (dxmax > 0.5) dxmax -= 1.0;
		while (dxmin < -0.5) dxmin += 1.0;
		while (dxmax < -0.5) dxmax += 1.0;
		if(dxmin > xmin) xmin = dxmin;
		if(dxmax < xmax) xmax = dxmax;
		
		if(xmax>xmin)
			prob += point.prob*(xmax-xmin);
	}
	
	return prob;
}

void AbcdSpaceProbabilityDistribution::CalculateProbabilityDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimits limits, 
																		int gridRes, int increment, bool normalize) {
	AbcdSpaceLimitsInt limsInt = limits.GenerateAbcdSpaceLimitsInt(gridRes);
	CalculateProbabilityDistribution(observedHotspots, limsInt, gridRes, increment, normalize);
}
	
void AbcdSpaceProbabilityDistribution::CalculateProbabilityDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimitsInt limsInt, 
																		int gridRes, int increment, bool normalize) {
	int LimitCount = HotspotCoords::NumLats*HotspotCoords::NumLongs*gridRes;
	
	std::vector<long int> starts;
	numProbPoints = CalculateNumberOfAbcdPoints(limsInt, gridRes, increment, &starts);

	probPoints = new AbcdSpacePoint[numProbPoints];
	
	long int totalCount = 0;
	#ifdef using_parallel
	#pragma omp parallel for
	#endif
	for (int ba = LimitCount - limsInt.limits[0][1] + increment; ba < limsInt.limits[1][0]; ba += increment) {
		long int count = 0;
		for (int ca = LimitCount - limsInt.limits[0][2] + increment; ca < limsInt.limits[2][0]; ca += increment) {
			for (int da = LimitCount - limsInt.limits[0][3] + increment; da < limsInt.limits[3][0]; da += increment) {
				if (ca-ba > LimitCount - limsInt.limits[1][2] && ca-ba < limsInt.limits[2][1] && 
					da-ba > LimitCount - limsInt.limits[1][3] && da-ba < limsInt.limits[3][1] &&
					da-ca > LimitCount - limsInt.limits[2][3] && da-ca < limsInt.limits[3][2]) {
					
					AbcdSpacePoint point((Double)ba/LimitCount, (Double)ca/LimitCount, (Double)da/LimitCount, 1.0);
					observedHotspots.Iterate(CalculateProbSingleHotspot, &point);
					
					int baIndex = (ba - (LimitCount - limsInt.limits[0][1] + increment))/increment;
					probPoints[count + starts[baIndex]] = point;
					count++;
				}
			}
		}
		#ifdef using_parallel
		#pragma omp critical
		#endif
		totalCount += count;
	}
	
	if(normalize) {
		Double sumProb = 0;
		for(long int i=0; i<numProbPoints; i++)
			sumProb += probPoints[i].prob;
		for(long int i=0; i<numProbPoints; i++)
			probPoints[i].prob /= sumProb;
	}
	
	if(numProbPoints != totalCount) {
		printf("Error: Anticipated %ld points, actually found %ld.\n", numProbPoints, totalCount);
		exit(EXIT_FAILURE);
	}
}

long int AbcdSpaceProbabilityDistribution::CalculateNumberOfAbcdPoints(AbcdSpaceLimits limits, int gridRes, int increment) {
	AbcdSpaceLimitsInt limsInt = limits.GenerateAbcdSpaceLimitsInt(gridRes);
	return CalculateNumberOfAbcdPoints(limsInt, gridRes, increment);
}

long int AbcdSpaceProbabilityDistribution::CalculateNumberOfAbcdPoints(AbcdSpaceLimitsInt limsInt, int gridRes, int increment, std::vector<long int>* starts) {
	int LimitCount = HotspotCoords::NumLats*HotspotCoords::NumLongs*gridRes;
	
	long int count = 0;
	for (int ba = LimitCount - limsInt.limits[0][1] + increment; ba < limsInt.limits[1][0]; ba += increment) {
		if (starts!=NULL)
			starts->push_back(count);
		for (int ca = LimitCount - limsInt.limits[0][2] + increment; ca < limsInt.limits[2][0]; ca += increment) {
			if (ca-ba > LimitCount - limsInt.limits[1][2] && ca-ba < limsInt.limits[2][1]){
				int minDa = LimitCount - limsInt.limits[0][3]; 
				int maxDa = limsInt.limits[3][0];
				
				int value = LimitCount - limsInt.limits[1][3] + ba;
				if(value > minDa)
					minDa += increment*((value - minDa)/increment);
				if(limsInt.limits[3][1] + ba < maxDa)
					maxDa = limsInt.limits[3][1] + ba;
				value = LimitCount - limsInt.limits[2][3] + ca;
				if(value > minDa)
					minDa += increment*((value - minDa)/increment);
				if(limsInt.limits[3][2] + ca < maxDa)
					maxDa = limsInt.limits[3][2] + ca;
					
				count += (maxDa-minDa-1)/increment;
			}
		}
	}
	return count;
}

void AbcdSpaceProbabilityDistribution::CalculateProbSingleHotspot(HotspotCoordsWithDate coord, void* data) {
	static const Double ScaleFactor = 3.2*HotspotCoords::NumLongs;
	
	AbcdSpacePoint* point = (AbcdSpacePoint*) data;
	
	if (point->prob == 0)
		return;
	
	Double xmin = -0.5/HotspotCoords::NumLats;
	Double xmax = 0.5/HotspotCoords::NumLats;
	
	Double a = ((Double)coord.moonLat)/HotspotCoords::NumLats;
	Double b = a + point->ba;
	Double c = a + point->ca;
	Double d = a + point->da;
	
	if (coord.moonLat == HotspotCoords::MissingCoord) {
		printf("Error: coord.moonLat is missing!\n");
		exit(EXIT_FAILURE);
	}
	
	if (coord.moonLong != HotspotCoords::MissingCoord) {
		Double bxmin = ((Double)coord.moonLong-0.5)/HotspotCoords::NumLongs - b;
		Double bxmax = ((Double)coord.moonLong+0.5)/HotspotCoords::NumLongs - b;
		while (bxmin > 0.5) bxmin -= 1.0;
		while (bxmax > 0.5) bxmax -= 1.0;
		while (bxmin < -0.5) bxmin += 1.0;
		while (bxmax < -0.5) bxmax += 1.0;
		if(bxmin > xmin) xmin = bxmin;
		if(bxmax < xmax) xmax = bxmax;
	}
	if (coord.marsLat != HotspotCoords::MissingCoord) {
		Double cxmin = ((Double)coord.marsLat-0.5)/HotspotCoords::NumLats - c;
		Double cxmax = ((Double)coord.marsLat+0.5)/HotspotCoords::NumLats - c;
		while (cxmin > 0.5) cxmin -= 1.0;
		while (cxmax > 0.5) cxmax -= 1.0;
		while (cxmin < -0.5) cxmin += 1.0;
		while (cxmax < -0.5) cxmax += 1.0;
		if(cxmin > xmin) xmin = cxmin;
		if(cxmax < xmax) xmax = cxmax;
	}
	if (coord.marsLong != HotspotCoords::MissingCoord) {
		Double dxmin = ((Double)coord.marsLong-0.5)/HotspotCoords::NumLongs - d;
		Double dxmax = ((Double)coord.marsLong+0.5)/HotspotCoords::NumLongs - d;
		while (dxmin > 0.5) dxmin -= 1.0;
		while (dxmax > 0.5) dxmax -= 1.0;
		while (dxmin < -0.5) dxmin += 1.0;
		while (dxmax < -0.5) dxmax += 1.0;
		if(dxmin > xmin) xmin = dxmin;
		if(dxmax < xmax) xmax = dxmax;
	}
	
	if(xmax>xmin)
		point->prob*=(xmax-xmin)*ScaleFactor;
	else {
		point->prob = 0;
	}
}
