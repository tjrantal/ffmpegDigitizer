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
#ifndef MARKERCOORDINATES_H
#define MARKERCOORDINATES_H
#include <vector>
#include "../trackingThread/TrackingThread.h"
	
	/**A struct to contain the information for a coordinate. Needs to be in a separate coordinate
	to create sortable vector...
	*/
	struct coordinate {
		double xCoordinate;		/**< X-coordinates*/
		double yCoordinate;		/**< Y-coordinates*/
		int frame;				/**< frame number of the coordinate*/
								/**Constructor*/
		coordinate(double xCIn, double yCIn, int frameIn) :xCoordinate(xCIn), yCoordinate(yCIn), frame(frameIn) {
		}
		/**Constructor overload*/
		coordinate() {
			xCoordinate = -1;
			yCoordinate = -1;
			frame = -1;
		}

		/** Enable sorting a vector with coordinate structs based on frame number*/
		bool operator < (const coordinate& coordinateToCompareTo) const {
			return (frame < coordinateToCompareTo.frame);
		}
	};

		/**
			 A struct to contain a histogram. Helps with memory management...
		*/
	struct Histogram {
	public:
		int channels;			/**histogram channels*/
		int bins;			/**histogram bins per channel*/
		double** histogram;
		Histogram(int _channels, int _bins) :channels(_channels), bins(_bins){
			histogram = new double*[3];
			for (int i = 0; i < channels; ++i) {
				histogram[i] = new double[bins]();	//Initialise to zero
			}
		}

		~Histogram() {
			for (int i = 0; i < channels; ++i) {
				delete[] histogram[i];
			}
			delete[] histogram;
			histogram = NULL;
		}

	};

	/**Structure for marker trajectories*/
	struct marker{
		wxString markerName;				/**< Marker name*/
		std::vector<coordinate> coordinates;	/**<a vector for coordinates*/
		int markerRadius;				/**< radius of the marker*/
		unsigned char colorImage;			/**< Color or BW image*/
		int colorTolerance;				/**< Color tolerance for marker region grow */
		int searchRadius;				/**< The radius of the search window*/
		unsigned char trackMarker;	/**< Is the marker being tracked*/
		unsigned char predictive;	/**< Trying to predict the marker trajectory*/
		int notFound;	/**< Was marker not found in the current frame*/
		std::vector<coordinate> *radiusCoordinates;	/**<Relative sampling coordinates around the marker*/
		std::vector<coordinate> *searchCoordinates;		/**<Relative sampling coordinates for track search area*/
		Histogram *histogram; 		/**<To store the colour histograms to look for*/
		unsigned char*	fourBitColors;		/**<To store the 4 highest bits of marker color for region grow*/
		int maxError;	/**<Maximum color error in a given channel for region growing*/
		/**Constructor*/
		marker(wxString markerNameIn):markerName(markerNameIn){
			/*Insert default values*/
			coordinates		= std::vector<coordinate>();	/*Coordinates*/
			markerRadius	= 15;					/* Size of LBP box*/
			colorImage	= (unsigned char) 1;	/* Color or BW image*/
			searchRadius	= 40;					/* The radius of the search window*/
			trackMarker	= (unsigned char) 1;	/* Is the marker being tracked*/
			predictive	= (unsigned char) 1;	/* Trying to predict the marker trajectory*/
			notFound		= (unsigned char) 0;	/* Was marker not found in the current frame*/
			histogram	=NULL;
			fourBitColors = NULL;
			colorTolerance = 127;
		}
	};
#endif
	
