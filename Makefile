PARALLEL = true

ifeq ($(PARALLEL), true)
	PARFLAGS =  -fopenmp -Dusing_parallel
else
	PARFLAGS =
endif

CC = g++
DEBUG = -g
CFLAGS = -Wall $(DEBUG) -O3 $(PARFLAGS)
LFLAGS = $(CFLAGS)
PROGS = CNmoonmars
SRCS = $(wildcard *.cpp)
INCL_OBJS = $(filter-out $(PROGS:%=%.o),$(SRCS:.cpp=.o))

.PHONY: all clean depend

all: $(PROGS)

$(PROGS): %: %.o $(INCL_OBJS)
	$(CC) $(LFLAGS) -o $@ $^

.cpp.o:
	$(CC) $(CFLAGS) -c $*.cpp

clean:
	rm -rf *.o *~ $(PROGS) Makefile.bak

depend:
	makedepend -Y -- $(SRCS)
# DO NOT DELETE

AbcdSpaceLimits.o: AbcdSpaceLimits.h ObservedHotspots.h
AbcdSpaceLimits.o: HotspotCoordsWithDate.h HotspotCoords.h Month.h
AbcdSpaceLimits.o: AbcdSpaceLimitsInt.h
AbcdSpaceLimitsInt.o: AbcdSpaceLimitsInt.h
AbcdSpaceProbabilityDistribution.o: AbcdSpaceProbabilityDistribution.h
AbcdSpaceProbabilityDistribution.o: ObservedHotspots.h
AbcdSpaceProbabilityDistribution.o: HotspotCoordsWithDate.h HotspotCoords.h
AbcdSpaceProbabilityDistribution.o: Month.h AbcdSpaceLimits.h
AbcdSpaceProbabilityDistribution.o: AbcdSpaceLimitsInt.h
CNmoonmars.o: ObservedHotspots.h HotspotCoordsWithDate.h HotspotCoords.h
CNmoonmars.o: Month.h AbcdSpaceLimits.h AbcdSpaceLimitsInt.h
CNmoonmars.o: AbcdSpaceProbabilityDistribution.h
CNmoonmars.o: PossibleHotspotsDistribution.h HotspotCoordsWithProbability.h
HotspotCoords.o: HotspotCoords.h
HotspotCoordsWithDate.o: HotspotCoordsWithDate.h HotspotCoords.h Month.h
HotspotCoordsWithProbability.o: HotspotCoordsWithProbability.h
HotspotCoordsWithProbability.o: HotspotCoords.h
Month.o: Month.h
ObservedHotspots.o: ObservedHotspots.h HotspotCoordsWithDate.h
ObservedHotspots.o: HotspotCoords.h Month.h
PossibleHotspotsDistribution.o: PossibleHotspotsDistribution.h
PossibleHotspotsDistribution.o: AbcdSpaceLimits.h ObservedHotspots.h
PossibleHotspotsDistribution.o: HotspotCoordsWithDate.h HotspotCoords.h
PossibleHotspotsDistribution.o: Month.h AbcdSpaceLimitsInt.h
PossibleHotspotsDistribution.o: AbcdSpaceProbabilityDistribution.h
PossibleHotspotsDistribution.o: HotspotCoordsWithProbability.h
