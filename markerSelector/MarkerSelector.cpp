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
			marker tempMarker(temp);	/* Marker name*/
			tempMarker.radiusCoordinates = getRelativeSamplingCoordinates((double) tempMarker.markerRadius);
			tempMarker.searchCoordinates = getRelativeSamplingCoordinates((double) tempMarker.searchRadius);
			//tempMarker.markerName 	= temp;					/* Marker name*/
			markers.push_back(tempMarker);			
			Append((const wxString) temp);
		}
	
	}
	currentMarker = 0;
	SetSelection(0);	//Select the first item on the list
	markerFile->Close();
	delete markerFile;
}

/**< Assign a value to a marker at a specific frame*/
void MarkerSelector::setCoordinate(int marker,double xCoordinate, double yCoordinate, int frameNo){
	/*Check that there is at least frameNo markers*/
	if (markers.size() > marker){
		/*Check whether the frame has coordinates already*/
		bool exists = false;
		for (int j = 0; j<markers[marker].coordinates.size();++j){
			if (frameNo == markers[marker].coordinates[j].frame){
				exists = true;
				markers[marker].coordinates[j].frame = frameNo;
				markers[marker].coordinates[j].xCoordinate = xCoordinate;
				markers[marker].coordinates[j].yCoordinate = yCoordinate;
			}
		}
		/*If the frame didn't have a coordinate yet, add*/
		if (exists == false){
			markers[marker].coordinates.push_back(coordinate(xCoordinate,yCoordinate,frameNo));
		}
		/*Sort the marker coordinates according to the frameNo*/
		std::sort(markers[marker].coordinates.begin(),markers[marker].coordinates.end());
	}
}

/**Set marker radius
	@param radius of the marker
*/
void MarkerSelector::setMarkerRadius(int markerNo, double radius){
	markers[markerNo].radiusCoordinates = getRelativeSamplingCoordinates(radius);
}

/**Set search radius
	@param radius of the search area
*/
void MarkerSelector::setSearchRadius(int markerNo, double radius){
	markers[markerNo].searchCoordinates = getRelativeSamplingCoordinates(radius);
}

/**< Get the coordinate at a specific frame*/
coordinate MarkerSelector::getCoordinate(int marker, int frameNo) throw(int){
	/*Check that there is at least frameNo markers*/
	if (markers.size() > marker && frameNo>-1){
		/*Check whether the frame has coordinates associated with it*/
		for (int j = 0; j<markers[marker].coordinates.size();++j){
			if (frameNo == markers[marker].coordinates[j].frame){
				return markers[marker].coordinates[j];
			}
		}
	}
	/*If the frame didn't have a marker throw and exception, int in this case*/
	throw(1);
	//return coordinate(-1,-1,-1);
}

/**Get relative sampling coordinate*/
std::vector<int*> MarkerSelector::getRelativeSamplingCoordinates(double radius){
	/*Calculate sampling coordinates for circular sampling*/
	std::vector<int*> samplingCoordinates = std::vector<int*>();
	for (int i = (int) floor(radius); i<(int) ceil(radius); ++i){
		for (int j = (int) floor(radius); j<(int) ceil(radius); ++j){
			if (((double) i)*((double) i)+((double) j)*((double) j)<=radius*radius){
				int* tempCoords = new int[2];
				tempCoords[0] = i;
				tempCoords[1] = j;
				samplingCoordinates.push_back(tempCoords);
			}
		}
	}
	return samplingCoordinates;
}

/**compare a histogram to the histogram of the marker*/
double MarkerSelector::getCloseness(double** histo1,double** histo2){
	double closeness = 0;
	for (int j = 0;j<3;++j){
		for (int i = 0;i<256;++i){
			closeness+= (histo1[j][i]<histo2[j][i]) ? histo1[j][i] : histo2[j][i];
		}
	}
	return closeness;
}

double min(){}


MarkerSelector::~MarkerSelector(){
	for (int j = 0; j<markers.size();++j){
		markers[j].coordinates.clear();
	}
	markers.clear();
}

