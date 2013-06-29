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


/** CLASS FOR READING MARKERS FROM A FILE AND DISPLAYING A MARKER LIST*/
class MarkerSelector : public wxComboBox{
	private:
		
	
	public:
		/**Structure for marker trajectories*/
		typedef struct{
			wxString markerName;				/**< Marker name*/
			std::vector<int> xCoordinates;	/**< X-coordinates*/
			std::vector<int> yCoordinates;	/**< Y-coordinates*/
			int lbpRadius;				/**< Size of LBP box*/
			unsigned char colorImage;			/**< Color or BW image*/
			int searchRadius;				/**< The radius of the search window*/
			unsigned char trackMarker;	/**< Is the marker being tracked*/
			unsigned char predictive;	/**< Trying to predict the marker trajectory*/
			int notFound;	/**< Was marker not found in the current frame*/
		} marker;
		
		/*Variables*/
		std::vector<marker> markers;	/**< Markers read from the file*/
		/*Functions*/
		/**Reset marker list dropdown 
		@param fileIn file to open*/
		void setMarkerList(wxString fileIn);	/**< Reset the marker list*/
		/**Consructor
		@param fileIn file to open
		@param parent Parent window to insert the dropdown menu to
		@param pos position of the menu
		@param size size of the menu
		*/
		MarkerSelector(wxString fileIn, DigitizerFrame* parent,wxWindowID id,const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize);
		
		/**Deconstructor*/
		/*~MarkerSelector();	/**< release memory*/

};

#endif