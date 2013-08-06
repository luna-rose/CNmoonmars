#include <cstdio>
#include <cstdlib>
#include "AbcdSpaceProbabilityDistribution.h"

AbcdSpaceProbabilityDistribution::AbcdSpaceProbabilityDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimits limits, 
																   int gridRes, int increment, bool normalize){
	LimitCount = HotspotCoords::NumLats*HotspotCoords::NumLongs*gridRes;
	CalculateProbabilityDistribution(observedHotspots, limits, gridRes, increment, normalize);
}

AbcdSpaceProbabilityDistribution::AbcdSpaceProbabilityDistribution(ObservedHotspots observedHotspots, AbcdSpaceLimitsInt limits, 
																   int gridRes, int increment, bool normalize){
	LimitCount = HotspotCoords::NumLats*HotspotCoords::NumLongs*gridRes;
	CalculateProbabilityDistribution(observedHotspots, limits, gridRes, increment, normalize);
}

AbcdSpaceProbabilityDistribution::~AbcdSpaceProbabilityDistribution() {
	delete[] probPoints;
}

long int AbcdSpaceProbabilityDistribution::GetNumPoints() {
	return numProbPoints;
}

void AbcdSpaceProbabilityDistribution::PrintToFile(std::string filename){
	FILE* file = fopen(filename.c_str(), "w");
	if(!file) {
		printf("Error: Could not open file for writing: \"%s\"\n", filename.c_str());
		exit(EXIT_FAILURE);
	}
	
	for(long int i=0; i<numProbPoints; i++){
		AbcdSpacePoint point = probPoints[i];
		fprintf(file, "%44.36Lf %44.36Lf %44.36Lf %47.36Le\n", ((Double)point.ba)/LimitCount,
				((Double)point.ca)/LimitCount, ((Double)point.da)/LimitCount, point.prob);
	}
	
	fclose(file);
	
	printf("Probability distribution contains %ld points.\n", numProbPoints);
	printf("Printed probability distribution to file: \"%s\".\n", filename.c_str());
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
		
		int latScale = LimitCount/HotspotCoords::NumLats;
		int longScale = LimitCount/HotspotCoords::NumLongs;
		
		int xmin = -latScale/2;
		int xmax = latScale - latScale/2;
		
		int a = coord.moonLat*latScale;
		int b = a + point.ba;
		int c = a + point.ca;
		int d = a + point.da;
		
		int bxmin = coord.moonLong*longScale - longScale/2 - b;
		int bxmax = coord.moonLong*longScale + longScale - longScale/2 - b;
		while (bxmin > LimitCount - LimitCount/2) bxmin -= LimitCount;
		while (bxmax > LimitCount - LimitCount/2) bxmax -= LimitCount;
		while (bxmin < -LimitCount/2) bxmin += LimitCount;
		while (bxmax < -LimitCount/2) bxmax += LimitCount;
		if(bxmin > xmin) xmin = bxmin;
		if(bxmax < xmax) xmax = bxmax;
		
		int cxmin = coord.marsLat*latScale - latScale/2 - c;
		int cxmax = coord.marsLat*latScale + latScale - latScale/2 - c;
		while (cxmin > LimitCount - LimitCount/2) cxmin -= LimitCount;
		while (cxmax > LimitCount - LimitCount/2) cxmax -= LimitCount;
		while (cxmin < -LimitCount/2) cxmin += LimitCount;
		while (cxmax < -LimitCount/2) cxmax += LimitCount;
		if(cxmin > xmin) xmin = cxmin;
		if(cxmax < xmax) xmax = cxmax;
		
		int dxmin = coord.marsLong*longScale - longScale/2 - d;
		int dxmax = coord.marsLong*longScale + longScale - longScale/2 - d;
		while (dxmin > LimitCount - LimitCount/2) dxmin -= LimitCount;
		while (dxmax > LimitCount - LimitCount/2) dxmax -= LimitCount;
		while (dxmin < -LimitCount/2) dxmin += LimitCount;
		while (dxmax < -LimitCount/2) dxmax += LimitCount;
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
	std::vector<long int> starts;
	numProbPoints = CalculateNumberOfAbcdPoints(limsInt, gridRes, increment, &starts);

	probPoints = new AbcdSpacePoint[numProbPoints];
	
	long int count = 0;
	for (int ba = LimitCount - limsInt.limits[0][1] + increment; ba < limsInt.limits[1][0]; ba += increment) {
		for (int ca = LimitCount - limsInt.limits[0][2] + increment; ca < limsInt.limits[2][0]; ca += increment) {
			for (int da = LimitCount - limsInt.limits[0][3] + increment; da < limsInt.limits[3][0]; da += increment) {
				if (ca-ba > LimitCount - limsInt.limits[1][2] && ca-ba < limsInt.limits[2][1] && 
					da-ba > LimitCount - limsInt.limits[1][3] && da-ba < limsInt.limits[3][1] &&
					da-ca > LimitCount - limsInt.limits[2][3] && da-ca < limsInt.limits[3][2]) {
					
					AbcdSpacePoint point(ba, ca, da, 1.0);
					probPoints[count] = point;
					count++;
				}
			}
		}
	}
	
	this->ComputeProbabilities(observedHotspots);
	
	if(normalize) {
		Normalize();
	}
	
	if(numProbPoints != count) {
		printf("Error: Anticipated %ld points, actually found %ld.\n", numProbPoints, count);
		exit(EXIT_FAILURE);
	}
}

void AbcdSpaceProbabilityDistribution::ComputeProbabilities(ObservedHotspots observedHotspots) {
	#ifdef using_parallel
	#pragma omp parallel for
	#endif
	for (long int i=0; i<numProbPoints; i++) {
		AbcdSpacePointWithLimitCount data;
		data.point = &probPoints[i];
		data.LimitCount = LimitCount;
		observedHotspots.Iterate(CalculateProbSingleHotspot, &data);
	}
}

void AbcdSpaceProbabilityDistribution::Normalize() {
	Double sumProb = 0;
	for(long int i=0; i<numProbPoints; i++)
		sumProb += probPoints[i].prob;
	for(long int i=0; i<numProbPoints; i++)
		probPoints[i].prob /= sumProb;
}

long int AbcdSpaceProbabilityDistribution::CalculateNumberOfAbcdPoints(AbcdSpaceLimits limits, int gridRes, int increment) {
	AbcdSpaceLimitsInt limsInt = limits.GenerateAbcdSpaceLimitsInt(gridRes);
	return CalculateNumberOfAbcdPoints(limsInt, gridRes, increment);
}

long int AbcdSpaceProbabilityDistribution::CalculateNumberOfAbcdPoints(AbcdSpaceLimitsInt limsInt, int gridRes, int increment, std::vector<long int>* starts) {
	int LimitCount = HotspotCoords::NumLats*HotspotCoords::NumLongs*gridRes;
	
	long int count = 0;
	for (int ba = LimitCount - limsInt.limits[0][1] + increment; ba < limsInt.limits[1][0]; ba += increment) {
		for (int ca = LimitCount - limsInt.limits[0][2] + increment; ca < limsInt.limits[2][0]; ca += increment) {
			if (starts!=NULL)
				starts->push_back(count);
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
	
	AbcdSpacePointWithLimitCount* ptLim = (AbcdSpacePointWithLimitCount*)data;
	AbcdSpacePoint* point = ptLim->point;
	int LimitCount = ptLim->LimitCount;
	
	int latScale = LimitCount/HotspotCoords::NumLats;
	int longScale = LimitCount/HotspotCoords::NumLongs;
	
	if (point->prob == 0)
		return;
	
	int xmin = -latScale/2;
	int xmax = latScale - latScale/2;
	
	int a = coord.moonLat*latScale;
	int b = a + point->ba;
	int c = a + point->ca;
	int d = a + point->da;
	
	if (coord.moonLat == HotspotCoords::MissingCoord) {
		printf("Error: coord.moonLat is missing!\n");
		exit(EXIT_FAILURE);
	}
	
	if (coord.moonLong != HotspotCoords::MissingCoord) {
		int bxmin = coord.moonLong*longScale - longScale/2 - b;
		int bxmax = coord.moonLong*longScale + longScale - longScale/2 - b;
		while (bxmin > LimitCount - LimitCount/2) bxmin -= LimitCount;
		while (bxmax > LimitCount - LimitCount/2) bxmax -= LimitCount;
		while (bxmin < -LimitCount/2) bxmin += LimitCount;
		while (bxmax < -LimitCount/2) bxmax += LimitCount;
		if(bxmin > xmin) xmin = bxmin;
		if(bxmax < xmax) xmax = bxmax;
	}
	if (coord.marsLat != HotspotCoords::MissingCoord) {
		int cxmin = coord.marsLat*latScale - latScale/2 - c;
		int cxmax = coord.marsLat*latScale + latScale - latScale/2 - c;
		while (cxmin > LimitCount - LimitCount/2) cxmin -= LimitCount;
		while (cxmax > LimitCount - LimitCount/2) cxmax -= LimitCount;
		while (cxmin < -LimitCount/2) cxmin += LimitCount;
		while (cxmax < -LimitCount/2) cxmax += LimitCount;
		if(cxmin > xmin) xmin = cxmin;
		if(cxmax < xmax) xmax = cxmax;
	}
	if (coord.marsLong != HotspotCoords::MissingCoord) {
		int dxmin = coord.marsLong*longScale - longScale/2 - d;
		int dxmax = coord.marsLong*longScale + longScale - longScale/2 - d;
		while (dxmin > LimitCount - LimitCount/2) dxmin -= LimitCount;
		while (dxmax > LimitCount - LimitCount/2) dxmax -= LimitCount;
		while (dxmin < -LimitCount/2) dxmin += LimitCount;
		while (dxmax < -LimitCount/2) dxmax += LimitCount;
		if(dxmin > xmin) xmin = dxmin;
		if(dxmax < xmax) xmax = dxmax;
	}
	
	if(xmax>xmin)
		point->prob*=(xmax-xmin)*ScaleFactor/LimitCount;
	else {
		point->prob = 0;
	}
}
