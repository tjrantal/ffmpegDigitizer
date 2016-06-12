
#ifndef COORDINATETRACKER_H
#define COORDINATETRACKER_H
#include <vector>
#include "MarkerCoordinates.h"
#include "../trackingThread/TrackingThread.h"

/**
		 A struct to contain a histogram. Helps with memory management...
	*/
class CoordinateTracker {
public:
	unsigned char* currentImage;
	int width;
	int height;
	int markerIndice;
	coordinate coordinates;
	coordinate foundCoord;
	Histogram * histogram;
	int colorTolerance;
	std::vector<coordinate> *samplingCoordinates;
	std::vector<coordinate> *searchCoordinates;

	/*Overload to track marker*/
	CoordinateTracker(unsigned char *_currentImage, int _width, int _height, int _markerIndice, coordinate _coordinates, Histogram* _histogram, int _colorTolerance, std::vector<coordinate> *_samplingCoordinates,	std::vector<coordinate> *_searchCoordinates):currentImage(_currentImage),width(_width),height(_height),markerIndice(_markerIndice),coordinates(_coordinates),histogram(_histogram),colorTolerance(_colorTolerance), samplingCoordinates(_samplingCoordinates), searchCoordinates(_searchCoordinates){
	}

	/*allow overriding with virtual*/
	virtual void getCoordinates() {
		try {
			foundCoord = TrackingThread::getMarkerCoordinates(currentImage, width, height, markerIndice, coordinates, histogram, colorTolerance, samplingCoordinates, searchCoordinates);
		}catch (...) {
			foundCoord = coordinate();
		}
	}
};

#endif
