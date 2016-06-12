/*
Copyright (C) 2013  Timo Rantalainen (tjrantal at gmail dot com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

For a copy of the GNU General Public License, see <http://www.gnu.org/licenses/>.
*/
#ifndef TRACKINGTHREAD_H
#define TRACKINGTHREAD_H

	#include "../digitizerFrame/DigitizerFrame.h"
	#include "../markerSelector/MarkerCoordinates.h"
	#include "../markerSelector/MarkerSelector.h"
	//#include "../markerSelector/TrackinThread.h"
	#include "../imagePanel/ImagePanel.h"
	#include "../videoReader/VideoReader.h"
	#include <vector>
	#include <algorithm>
	#include <thread>			//For threading, tracking opens up a new thread, requires ISO C++ 2011 compliant compiler, e.g. g++ version 4.7 and up
	#include <stdio.h>			//For output
	//#include <unistd.h>			//For sleep
	#include <wx/image.h>		//For working with the image
	#include <wx/bitmap.h>		//For working with the image
	#include <math.h>			//For M_PI
	#include <thread>			//For sleeping
	#include <chrono>			//For millisecodns sleep

//Forward declare 
struct Histogram;

	/**Struct used to store coordinates for the closeness measure*/
	struct coordinateCloseness{
		double x;
		double y;
		double closeness;
		/**Constructor*/
		coordinateCloseness(double xIn, double yIn, double closenessIn):x(xIn),y(yIn),closeness(closenessIn){
		}
		/**Constructor overload*/
		coordinateCloseness(){			
			x			= -1.0;
			y			= -1.0;
			closeness	= -1.0;
		}
	
		/** Enable sorting a vector with coordinateCloseness structs based on frame number*/
		bool operator < (const coordinateCloseness& coordinateClosenessToCompareTo) const {
			return (closeness < coordinateClosenessToCompareTo.closeness);
		}
	};

	
	class TrackingThread{
		private:
			DigitizerFrame* mainThread;
		public:
			std::thread tThread;
			/**Method to start the thread*/
			void startThread();
			/**Method to feed into the thread constructor*/
			void run();
			/**Look for the marker in the image based on histograms*/
			coordinate getMarkerCoordinates(wxImage *currentImage,int markerIndice, coordinate coordinates, double** histogram);
			static coordinate getMarkerCoordinates(unsigned char *currentImage, int width, int height, int markerIndice, coordinate coordinates, Histogram* histogram,int colorTolerance, std::vector<coordinate> *samplingCoordinates, std::vector<coordinate> *searchCoordinates) throw(int);
			/**Optical flow assisted marker search*/
			static coordinate getFlow(unsigned char *prevImage,unsigned char *currentImage, int width, int height,coordinate coordinates,std::vector<coordinate> *searchCoordinates);
			static std::vector<coordinate> getMarkerCoordinatesRegionGrow(unsigned char *currentImage, int width, int height, coordinate startSearch,unsigned char *markerColor, int colorTolerance,std::vector<coordinate> *searchCoordinates,double markerRadius) throw(int);
			
			/**Look for the marker in the image based on region growing*/
			std::vector<coordinate> getMarkerCoordinatesRegionGrow(wxImage *currentImage,int markerIndice, coordinate coordinates) throw(int);
			std::vector<coordinate> getMarkerCoordinatesRegionGrow(unsigned char *currentImage,int markerIndice, coordinate coordinates) throw(int);
			
			/**Get the histogram of the current marker*/
			/*
			static double** getHistogram(wxImage *currentImage,coordinate coordinates, std::vector<coordinate> samplingCoordinates);
			static double** getHistogram(unsigned char *currentImage, int width, int height, coordinate coordinates, std::vector<coordinate> samplingCoordinates);
			*/
			static Histogram* getHistogram16(unsigned char *currentImage, int width, int height, coordinate coordinates, std::vector<coordinate> samplingCoordinates);

			/**Get the color of the current marker*/
			static unsigned char* getColor(wxImage *currentImage,int xCoordinate,int yCoordinate);	
			static unsigned char* getColor(unsigned char *currentImage, int width, int height,int xCoordinate,int yCoordinate);	
			
			/**Region grow algorithm*/
			static std::vector<coordinate> growRegion(wxImage *currentImage,double x, double y, unsigned char* markerColor, int colorTolerance);
			static std::vector<coordinate> growRegion(unsigned char *currentImage,int imageWidth,int imageHeight,double x, double y, unsigned char* markerColor, int colorTolerance);
			
			
			/**Constructor*/
			TrackingThread(DigitizerFrame* mainThreadIn);
	};
#endif
