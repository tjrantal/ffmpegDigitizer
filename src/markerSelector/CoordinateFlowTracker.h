
#ifndef COORDINATEFLOWTRACKER_H
#define COORDINATEFLOWTRACKER_H
#include <vector>
#include "MarkerCoordinates.h"
#include "../trackingThread/TrackingThread.h"

/**
		 A struct to contain parameters for flow tracker. Enables using threads...
	*/
class CoordinateFlowTracker: public CoordinateTracker {
public:
	unsigned char *prevImage;
	unsigned char *markerColor;
	double markerRadius;

	/*Overload to track marker*/
	CoordinateFlowTracker(unsigned char *_currentImage, int _width, int _height, int _markerIndice, coordinate _coordinates, Histogram* _histogram, int _colorTolerance, std::vector<coordinate> *_samplingCoordinates,	std::vector<coordinate> *_searchCoordinates,unsigned char *_prevImage,unsigned char* _markerColor,double _markerRadius):CoordinateTracker(_currentImage,_width,_height,_markerIndice,_coordinates,_histogram,_colorTolerance,_samplingCoordinates,_searchCoordinates)
{
				prevImage = _prevImage;
		markerColor = _markerColor;
		markerRadius = _markerRadius;
	}

	/*override the parent class getCoordinates..*/
	void getCoordinates() {
		try {
			//Calculate optical flow to get an initial guess
			coordinate startSearch = TrackingThread::getFlow(prevImage,currentImage,width,height,coordinates,searchCoordinates);
			std::vector<coordinate> areaCoordinates = TrackingThread::getMarkerCoordinatesRegionGrow(currentImage, width, height, startSearch,markerColor, colorTolerance,searchCoordinates,markerRadius);
			//Calculate the mean of the area coordinates..
			double meanCoord[2] = { 0,0 };	//init to zero
			double meanSize = areaCoordinates.size();
			for (int j = 0; j < areaCoordinates.size(); ++j) {
				meanCoord[0] += areaCoordinates[j].xCoordinate / meanSize;
				meanCoord[1] += areaCoordinates[j].yCoordinate / meanSize;
			}
			
			
			foundCoord = coordinate(meanCoord[0],meanCoord[1],-1);
		}catch (...) {
			foundCoord = coordinate();
		}
	}
};

#endif
