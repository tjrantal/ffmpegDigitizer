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
	#include "TrackingThread.h"
#endif

TrackingThread::TrackingThread(DigitizerFrame* mainThreadIn){
	mainThread = mainThreadIn;
}

void TrackingThread::startThread(){
	tThread = std::thread(&TrackingThread::run,this);
}

void TrackingThread::run(){

	/*Get the current image*/
	wxImage currentImage = mainThread->imagePanel->currentClearImage;
	int currentFrame = mainThread->slider->GetValue();
	
	//coordinate coordinatesReturned[mainThread->markerSelector->markers.size()];
	mainThread->SetStatusText(wxString::Format(wxT("%s %d"),_("Thread started, frame #"), currentFrame));
	//Go through the frames in the video
	while (mainThread->trackOn == true && currentFrame < mainThread->videoReader->getNumberOfIndices()){
		mainThread->SetStatusText(wxString::Format(wxT("%s %d"),_("In loop, frame #"), currentFrame));
		//Go through all of the markers in the image
		for (int i = 0; i<mainThread->markerSelector->markers.size();++i){
			//Look for coordinate in the previous image/
			coordinate* coordinatesReturned;
			try{
				coordinate initCoordinate = mainThread->markerSelector->getCoordinate(i, currentFrame-1);
				getMarkerCoordinates(currentImage,i,coordinatesReturned, initCoordinate, mainThread->markerSelector->markers[i].histogram);
				mainThread->markerSelector->setCoordinate(i,coordinatesReturned[i].xCoordinate, coordinatesReturned[i].yCoordinate, currentFrame);
				//Digitize the marker
				mainThread->imagePanel->digitizeXY(coordinatesReturned[i].xCoordinate,coordinatesReturned[i].yCoordinate, (double) mainThread->markerSelector->markers[i].markerRadius);
			} catch (int err){	//Didn't have marker in previous frame, check the current frame
				try{
					coordinate initCoordinate = mainThread->markerSelector->getCoordinate(i, currentFrame);
					getMarkerCoordinates(currentImage,i,coordinatesReturned, initCoordinate, mainThread->markerSelector->markers[i].histogram);
					mainThread->markerSelector->setCoordinate(i,coordinatesReturned[i].xCoordinate, coordinatesReturned[i].yCoordinate, currentFrame);
					//Digitize the marker
					mainThread->imagePanel->digitizeXY(coordinatesReturned[i].xCoordinate,coordinatesReturned[i].yCoordinate, (double) mainThread->markerSelector->markers[i].markerRadius);
				} catch (int err){
					//Marker has not been digitized in the previous or the current frame, so do nothing for this marker
				}

			}
		}
		
		//Advance frame
		currentFrame++;
		mainThread->slider->SetValue(currentFrame);
		mainThread->videoReader->readFrameFromDisk(currentFrame);
		mainThread->imagePanel->setImage(mainThread->videoReader->width,mainThread->videoReader->height,mainThread->videoReader->decodedFrame,true);
	}
	/*Digitize the last frame as well*/
	if (mainThread->trackOn == true){
		/*Digitize the last frame here*/
	}
}

/**Look for the marker in the image*/
void TrackingThread::getMarkerCoordinates(wxImage currentImage,int markerIndice,coordinate* returnCoordinate, coordinate coordinates, double** histogram){
	/*Go through the search area, check the closeness of each of the marker-sized histograms vs the marker histogram.
	Digitize the closest match, provided that it is above the threshold.*/
	double** markerHistogram = mainThread->markerSelector->markers[markerIndice].histogram;
	std::vector<coordinateCloseness> checkClose;
	std::vector<int*> samplingCoordinates = mainThread->markerSelector->markers[markerIndice].radiusCoordinates;
	std::vector<int*> searchCoordinates = mainThread->markerSelector->markers[markerIndice].searchCoordinates;
	for (int i = 0;i<searchCoordinates.size();++i){
		double x = coordinates.xCoordinate+searchCoordinates[i][0];
		double y = coordinates.yCoordinate+searchCoordinates[i][1];
		double closeness = mainThread->markerSelector->getCloseness(markerHistogram,getHistogram(currentImage,coordinates,samplingCoordinates));
		checkClose.push_back(coordinateCloseness(x,y,closeness));
	}
	
	std::sort(checkClose.begin(),checkClose.end());	//Sort the closeness values to ascending order, best closeness is the last
	coordinateCloseness bestMatch = checkClose.back();
	returnCoordinate = new coordinate(bestMatch.x,bestMatch.y,-1);
	/*
	mainThread->SetStatusText(wxString::Format(wxT("%s %d"),_("looking for marker coordinate marker #"), markerIndice));
	sleep(1);
	*/
}

/**Get the histogram of the current marker*/
double** TrackingThread::getHistogram(wxImage currentImage,coordinate coordinates, std::vector<int*> samplingCoordinates){

	/*Get the histogram*/
	double** histogram;
	histogram = new double*[3];	/*Color figure comprises 3 different colors...*/
	for (int i = 0; i<3;++i){
		histogram[i] = new double[256](); /*256 possible intensities of a given color, the () initialises the values to zero*/
	}
	int xCoordinate = coordinates.xCoordinate;
	int yCoordinate = coordinates.yCoordinate;
	/*get the colorvalues for the histograms*/
	for (int i = 0; i<samplingCoordinates.size(); ++i){
		if (xCoordinate+samplingCoordinates[i][0] >=0 && xCoordinate+samplingCoordinates[i][0] < currentImage.GetWidth()
			&& xCoordinate+samplingCoordinates[i][1] >=0 && xCoordinate+samplingCoordinates[i][1] < currentImage.GetHeight()
			){
			histogram[0][currentImage.GetRed(xCoordinate+samplingCoordinates[i][0],yCoordinate+samplingCoordinates[i][1])]		+= 1;
			histogram[1][currentImage.GetGreen(xCoordinate+samplingCoordinates[i][0],yCoordinate+samplingCoordinates[i][1])]	+= 1;
			histogram[2][currentImage.GetBlue(xCoordinate+samplingCoordinates[i][0],yCoordinate+samplingCoordinates[i][1])]	+= 1;
		}
	}
	/*Normalize sum to 1 (maximum, next to border sum of histogram will be less than 0*/
	for (int j = 0;j<3;++j){
		for (int i = 0;i<256;++i){
			histogram[j][i] /= ((double) samplingCoordinates.size());
		}
	}
	return histogram;
}	
