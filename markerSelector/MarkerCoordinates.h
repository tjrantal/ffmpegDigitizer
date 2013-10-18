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
	
	/**A struct to contain the information for a coordinate. Needs to be in a separate coordinate
		to create sortable vector...
	*/
	struct coordinate{
		double xCoordinate;		/**< X-coordinates*/
		double yCoordinate;		/**< Y-coordinates*/
		int frame;				/**< frame number of the coordinate*/
		/**Constructor*/
		coordinate(double xCIn, double yCIn, int frameIn):xCoordinate(xCIn),yCoordinate(yCIn),frame(frameIn){
		}
		/**Constructor overload*/
		coordinate(){			
			xCoordinate	= -1;
			yCoordinate	= -1;
			frame		= -1;
		}
		
		/** Enable sorting a vector with coordinate structs based on frame number*/
		bool operator < (const coordinate& coordinateToCompareTo) const {
			return (frame < coordinateToCompareTo.frame);
		}			
	};
	/**Structure for marker trajectories*/
	struct marker{
		wxString markerName;				/**< Marker name*/
		std::vector<coordinate> coordinates;	/**<a vector for coordinates*/
		int markerRadius;				/**< radius of the marker*/
		unsigned char colorImage;			/**< Color or BW image*/
		int searchRadius;				/**< The radius of the search window*/
		unsigned char trackMarker;	/**< Is the marker being tracked*/
		unsigned char predictive;	/**< Trying to predict the marker trajectory*/
		int notFound;	/**< Was marker not found in the current frame*/
		std::vector<coordinate> *radiusCoordinates;	/**<Relative sampling coordinates around the marker*/
		std::vector<coordinate> *searchCoordinates;		/**<Relative sampling coordinates for track search area*/
		double** histogram; 		/**<To store the colour histograms to look for*/
		/**Constructor*/
		marker(wxString markerNameIn):markerName(markerNameIn){
			/*Insert default values*/
			coordinates		= std::vector<coordinate>();	/*Coordinates*/
			markerRadius	= 10;					/* Size of LBP box*/
			colorImage	= (unsigned char) 1;	/* Color or BW image*/
			searchRadius	= 30;					/* The radius of the search window*/
			trackMarker	= (unsigned char) 1;	/* Is the marker being tracked*/
			predictive	= (unsigned char) 1;	/* Trying to predict the marker trajectory*/
			notFound		= (unsigned char) 0;	/* Was marker not found in the current frame*/
			histogram	=NULL;
		}
	};
#endif
	