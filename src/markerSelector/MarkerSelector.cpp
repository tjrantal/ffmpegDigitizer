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
#include <string>      
#include <sstream>

MarkerSelector::MarkerSelector(wxString fileIn, DigitizerFrame* parent,wxWindowID id,const wxPoint &pos,const wxSize &size) : wxComboBox((wxWindow *)parent,id,_(""),pos,size,0,NULL,wxCB_DROPDOWN){
	setMarkerList(fileIn);	/*Set the marker list*/
}

void MarkerSelector::setMarkerList(wxString fileIn){
	wxTextFile* markerFile = new wxTextFile();
	markerFile->Open(fileIn,wxConvUTF8); /*Open the file for reading*/
	wxString nextLine;
	Clear();	/*Remove pre-existing items*/
	markers.clear();
	//markers = new std::vector<wxString>();
	/*Read markers from the list and add them to the dropdown menu*/
	int i = 0;
	printf("Start reading markers\n");
	if (markerFile->IsOpened()) {
		for (int i = 0; i < markerFile->GetLineCount();++i) {
			nextLine = markerFile->GetLine(i);
			std::vector<std::string> delimited;
			std::string tempLine(nextLine);
			std::stringstream sourceStringStream(tempLine);
			std::string item;
			while (std::getline(sourceStringStream, item, '\t')) {
				delimited.push_back(item);
				printf("%s\t",item);
			}
			marker tempMarker(delimited.at(0));	/* Marker name*/
			Append((const wxString) delimited.at(0));
			if (delimited.size()>1){
				tempMarker.markerRadius = std::stoi(delimited.at(1), nullptr);
				tempMarker.searchRadius = std::stoi(delimited.at(2), nullptr);
				tempMarker.colorTolerance = std::stoi(delimited.at(3), nullptr);
				
			}
			tempMarker.radiusCoordinates = getRelativeSamplingCoordinates((double)tempMarker.markerRadius);
			tempMarker.searchCoordinates = getRelativeSamplingCoordinates((double)tempMarker.searchRadius);
			markers.push_back(tempMarker);
			printf("\n");
		}
	}


	/*
	if (markerFile->IsOpened()){
		for (temp = markerFile->GetFirstLine(); !markerFile ->Eof(); temp = markerFile->GetNextLine()){
			marker tempMarker(temp);	// Marker name
			tempMarker.radiusCoordinates = getRelativeSamplingCoordinates((double) tempMarker.markerRadius);
			tempMarker.searchCoordinates = getRelativeSamplingCoordinates((double) tempMarker.searchRadius);
			//tempMarker.markerName 	= temp;					// Marker name
			markers.push_back(tempMarker);			
			Append((const wxString) temp);
		}
	
	}
	*/
	printf("Done reading markers\n");
	currentMarker = 0;
	SetSelection(0);	//Select the first item on the list
	markerFile->Close();
	delete markerFile;
	printf("Done with reading markers\n");
}

/**< Assign a value to a marker at a specific frame*/
void MarkerSelector::setCoordinate(int markerIndice,double xCoordinate, double yCoordinate, int frameNo){
	/*Check that there is at least frameNo markers*/
	if (markers.size() > markerIndice){
		/*Check whether the frame has coordinates already*/
		bool exists = false;
		for (int j = 0; j<markers[markerIndice].coordinates.size();++j){
			if (frameNo == markers[markerIndice].coordinates[j].frame){
				exists = true;
				markers[markerIndice].coordinates[j].frame = frameNo;
				markers[markerIndice].coordinates[j].xCoordinate = xCoordinate;
				markers[markerIndice].coordinates[j].yCoordinate = yCoordinate;
			}
		}
		/*If the frame didn't have a coordinate yet, add*/
		if (exists == false){
			markers[markerIndice].coordinates.push_back(coordinate(xCoordinate,yCoordinate,frameNo));
		}
		/*Sort the marker coordinates according to the frameNo*/
		std::sort(markers[markerIndice].coordinates.begin(),markers[markerIndice].coordinates.end());
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
coordinate MarkerSelector::getCoordinate(int markerIndice, int frameNo) throw(int){
	/*Check that there is at least frameNo markers*/
	if (markers.size() > markerIndice && frameNo>-1){
		/*Check whether the frame has coordinates associated with it*/
		for (int j = 0; j<markers[markerIndice].coordinates.size();++j){
			if (frameNo == markers[markerIndice].coordinates[j].frame){
				return markers[markerIndice].coordinates[j];
			}
		}
	}
	/*If the frame didn't have a marker throw an exception*/
	throw(1);
}

/**Get relative sampling coordinate*/
std::vector<coordinate> *MarkerSelector::getRelativeSamplingCoordinates(double radius){
	/*Calculate sampling coordinates for circular sampling*/
	/*Expand from the centre of search area*/
	std::vector<coordinate> *samplingCoordinates = new std::vector<coordinate>();
	samplingCoordinates->push_back(coordinate(0,0,-1));
	int targetRadius = (int) ceil(radius);
	int currentRadius = 1;
	int i, j;
	/*check the next layer of surrounding pixels*/
	while (currentRadius <= targetRadius){
		/*Need to fill four sides, top left to right, right top to bottom, bottom left to right and left bottom to top*/
		/*top and bottom left to right*/
		j = currentRadius;
		for (i = -currentRadius;i<=currentRadius;++i){
			if (((double) i)*((double) i)+((double) j)*((double) j)<=radius*radius){
				samplingCoordinates->push_back(coordinate(i,j,-1));		/*top*/
				samplingCoordinates->push_back(coordinate(i,-j,-1));	/*bottom*/
			}
		}
		/*Left and right top to bottom, ex top and bottom rows*/
		i = currentRadius;
		for (j = -currentRadius+1;j<currentRadius;++j){
			if (((double) i)*((double) i)+((double) j)*((double) j)<=radius*radius){
				samplingCoordinates->push_back(coordinate(i,j,-1));		/*right*/
				samplingCoordinates->push_back(coordinate(-i,j,-1));	/*left*/
			}
		}
		++currentRadius;
	}
	return samplingCoordinates;
}

/**compare a histogram to the histogram of the marker*/
double MarkerSelector::getCloseness(Histogram* histo1, Histogram* histo2){
	double closeness = 0;
	for (int j = 0;j<3;++j){
		for (int i = 0;i<256;++i){
			closeness+= (histo1->histogram[j][i]<histo2->histogram[j][i]) ? histo1->histogram[j][i] : histo2->histogram[j][i];
		}
	}
	return closeness;
}

double MarkerSelector::getCloseness16(Histogram* histo1, Histogram* histo2){
	double closeness = 0;
	for (int j = 0;j<3;++j){
		for (int i = 0;i<16;++i){
			closeness+= (histo1->histogram[j][i]<histo2->histogram[j][i]) ? histo1->histogram[j][i] : histo2->histogram[j][i];
		}
	}
	return closeness;
}

void MarkerSelector::eraseMarker(int markerIndice) {
	markers[markerIndice].coordinates.clear();
}

void MarkerSelector::eraseCoordinate(int markerIndice, int frameNo){
	/*Check that there is at least frameNo markers*/
	if (markers.size() > markerIndice && frameNo>-1){
		/*Check whether the frame has coordinates associated with it*/
		for (int j = 0; j<markers[markerIndice].coordinates.size();++j){
			if (frameNo == markers[markerIndice].coordinates[j].frame){
				markers[markerIndice].coordinates.erase(markers[markerIndice].coordinates.begin()+j);
				return;
			}
		}
	}
}

MarkerSelector::~MarkerSelector(){
	for (int j = 0; j<markers.size();++j){
		markers[j].coordinates.clear();
	}
	markers.clear();
}


