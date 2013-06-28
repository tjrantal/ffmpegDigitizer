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
	
	wxTextFile* markerFile = new wxTextFile();
	markerFile->open(fileIn,wxConvUTF8); /*Open the file for reading*/
	wxString temp;
	tempMarkers = new std::vector<wxString>();
	if (markerFile->IsOpen()){
		for (temp = markerFile->GetFistLine(); !markerFile ->EoF(); temp = markerFile->GetNextLine()){
			tempMarkers.push_back(temp);
		}
	
	}
	std::copy(tempMarkers.begin(), tempMarkers.end(), markers);
}

MarkerSelector::~MarkerSelector(){}