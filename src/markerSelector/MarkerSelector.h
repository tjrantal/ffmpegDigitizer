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
#include "MarkerCoordinates.h"
#include <vector>
#include <algorithm>

//Declare struct marker
struct marker;
struct Histogram;

/** CLASS FOR READING MARKERS FROM A FILE AND DISPLAYING A MARKER LIST*/
class MarkerSelector : public wxComboBox{
	private:
		
	
	public:
		
		/*Variables*/
		std::vector<marker> markers;	/**< Markers read from the file*/
		int currentMarker;
		/*Functions*/
		/**Reset marker list dropdown 
		@param fileIn file to open*/
		void setMarkerList(wxString fileIn);	/**< Reset the marker list*/
		/** Set the coordinates of a given marker
		@param markerIndice the indice of the marker to set
		@param xCoordinate the x-coordinate of the marker
		@param yCoordinate the y-coordinate of the marker
		@param frameNo the frame for which the coordinates are assigned to*/
		void setCoordinate(int markerIndice,double xCoordinate, double yCoordinate, int frameNo);	/**< Assign a value to a marker at a specific frame*/
		/** Get the coordinates of a given marker in a specific frame
		@param markerIndice the indice of the marker to get
		@param frameNo the frame for which the coordinate is to be looked up for*/
		coordinate getCoordinate(int markerIndice, int frameNo) throw(int);	/**< Get the coordinate at a specific frame*/
		
		/**Set relative sampling coordinate
			@param radius of the circular sampling area
		*/
		std::vector<coordinate> *getRelativeSamplingCoordinates(double radius);	/**Set sampling area*/
		
		/**Set marker radius
			@param radius of the marker
		*/
		void setMarkerRadius(int markerNo, double radius);	/**Set marker radius*/
		
		/**Set search radius
			@param radius of the search area
		*/
		void setSearchRadius(int markerNo, double radius);	/**Set search*/
		
		/**Calculate histogram closeness
			@param histo1 First histogram for the closeness
			@param histo2 Second histogram for the closeness
		*/
		double getCloseness(Histogram* histo1, Histogram* histo2);	/**compare a histogram to the histogram of the marker*/
		
		static double getCloseness16(Histogram* histo1, Histogram* histo2);

		/**
		 * @brief Clear marker frameNo
		 * @param markerIndice marker to erase
		 * @param frameNo frame to erase the marker from
		 */
		void eraseCoordinate(int markerIndice, int frameNo);
		
		/**
		 * @brief clear marker coordinates to 
		 * @param markerIndice the marker to reset
		 */		
		void eraseMarker(int markerIndice);
		
		/**Consructor
		@param fileIn file to open
		@param parent Parent window to insert the dropdown menu to
		@param pos position of the menu
		@param size size of the menu
		*/
		MarkerSelector(wxString fileIn, DigitizerFrame* parent,wxWindowID id,const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize);
		/**Deconstructor*/
		~MarkerSelector();
};

#endif
