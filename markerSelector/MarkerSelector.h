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
#ifndef MARKERSELECTOR_H
#define MARKERSELECTOR_H

#include <wx/wx.h>
#include <wx/textfile.h>
#include "../digitizerFrame/DigitizerFrame.h"
#include <vector>
#include <algorithm>

/** CLASS FOR READING MARKERS FROM A FILE AND DISPLAYING A MARKER LIST*/
class MarkerSelector : public wxComboBox{
	private:
		
	
	public:
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
				
			}
		};
		
		/*Variables*/
		std::vector<marker> markers;	/**< Markers read from the file*/
		/*Functions*/
		/**Reset marker list dropdown 
		@param fileIn file to open*/
		void setMarkerList(wxString fileIn);	/**< Reset the marker list*/
		/** Set the coordinates of a given marker
		@param marker the indice of the marker to set
		@param xCoordinate the x-coordinate of the marker
		@param yCoordinate the y-coordinate of the marker
		@param frameNo the frame for which the coordinates are assigned to*/
		void setCoordinate(int marker,double xCoordinate, double yCoordinate, int frameNo);	/**< Assign a value to a marker at a specific frame*/
		/** Get the coordinates of a given marker in a specific frame
		@param marker the indice of the marker to get
		@param frameNo the frame for which the coordinate is to be looked up for*/
		coordinate getCoordinate(int marker, int frameNo);	/**< Get the coordinate at a specific frame*/
		/**Consructor
		@param fileIn file to open
		@param parent Parent window to insert the dropdown menu to
		@param pos position of the menu
		@param size size of the menu
		*/
		MarkerSelector(wxString fileIn, DigitizerFrame* parent,wxWindowID id,const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize);
};

#endif
