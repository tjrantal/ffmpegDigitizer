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
	#include "MarkerSelector.h"
#endif
MarkerSelector::MarkerSelector(wxString fileIn, DigitizerFrame* parent,wxWindowID id,const wxPoint &pos,const wxSize &size) : wxComboBox((wxWindow *)parent,id,_(""),pos,size,0,NULL,wxCB_DROPDOWN){
	setMarkerList(fileIn);	/*Set the marker list*/
}

void MarkerSelector::setMarkerList(wxString fileIn){
	wxTextFile* markerFile = new wxTextFile();
	markerFile->Open(fileIn,wxConvUTF8); /*Open the file for reading*/
	wxString temp;
	Clear();	/*Remove pre-existing items*/
	markers.clear();
	//markers = new std::vector<wxString>();
	/*Read markers from the list and add them to the dropdown menu*/
	if (markerFile->IsOpened()){
		for (temp = markerFile->GetFirstLine(); !markerFile ->Eof(); temp = markerFile->GetNextLine()){
			marker tempMarker;
			tempMarker.markerName 	= temp;					/* Marker name*/
			tempMarker.xCoordinates	= std::vector<int>();	/* X-coordinates*/
			tempMarker.yCoordinates	= std::vector<int>();	/* Y-coordinates*/
			tempMarker.lbpRadius	= 10;					/* Size of LBP box*/
			tempMarker.colorImage	= (unsigned char) 1;	/* Color or BW image*/
			tempMarker.searchRadius	= 30;					/* The radius of the search window*/
			tempMarker.trackMarker	= (unsigned char) 1;	/* Is the marker being tracked*/
			tempMarker.predictive	= (unsigned char) 1;	/* Trying to predict the marker trajectory*/
			tempMarker.notFound		= (unsigned char) 0;	/* Was marker not found in the current frame*/
			markers.push_back(tempMarker);			
			Append((const wxString) temp);
		}
	
	}
	SetSelection(0);	//Select the first item on the list
	markerFile->Close();
	delete markerFile;
}

/*
MarkerSelector::~MarkerSelector(){
	markers.clear();
	delete &markers;
}
*/
