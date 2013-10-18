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
	#include "../imagePanel/ImagePanel.h"
	#include "../videoReader/VideoReader.h"
	#include <vector>
	#include <algorithm>
	#include <thread>			//For threading, tracking opens up a new thread, requires ISO C++ 2011 compliant compiler, e.g. g++ version 4.7 and up
	#include <stdio.h>			//For output
	#include <unistd.h>			//For sleep
	#include <wx/image.h>		//For working with the image
	#include <wx/bitmap.h>		//For working with the image

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
			/**Look for the marker in the image*/
			coordinate getMarkerCoordinates(wxImage currentImage,int markerIndice, coordinate coordinates, double** histogram);
			/**Get the histogram of the current marker*/
			double** getHistogram(wxImage currentImage,coordinate coordinates, std::vector<coordinate> samplingCoordinates);
			/**Constructor*/
			TrackingThread(DigitizerFrame* mainThreadIn);
	};
#endif