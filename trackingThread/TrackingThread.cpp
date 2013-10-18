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

	int currentFrame = mainThread->slider->GetValue();
	//coordinate coordinatesReturned[mainThread->markerSelector->markers.size()];
	//mainThread->SetStatusText(wxString::Format(wxT("%s %d"),_("Thread started, frame #"), currentFrame));
	//Go through the frames in the video
	while (mainThread->trackOn == true && currentFrame < mainThread->videoReader->getNumberOfIndices()){
		/*Get the current image*/
		wxImage currentImage = mainThread->imagePanel->currentClearImage;
		//mainThread->SetStatusText(wxString::Format(wxT("%s %d"),_("In loop, frame #"), currentFrame));
		//Go through all of the markers in the image
		for (int i = 0; i<mainThread->markerSelector->markers.size();++i){
			//Look for coordinate in the previous image/
			
			try{
				coordinate initCoordinate = mainThread->markerSelector->getCoordinate(i, currentFrame-1);
				coordinate coordinatesReturned = getMarkerCoordinates(currentImage,i, initCoordinate, mainThread->markerSelector->markers[i].histogram);
				//mainThread->SetStatusText(wxString::Format(wxT("%s %d"),_("Returned current-1 "), i));
				//	sleep(1);
				mainThread->markerSelector->setCoordinate(i,coordinatesReturned.xCoordinate, coordinatesReturned.yCoordinate, currentFrame);
				//Digitize the marker
				mainThread->imagePanel->digitizeXY(coordinatesReturned.xCoordinate,coordinatesReturned.yCoordinate, (double) mainThread->markerSelector->markers[i].markerRadius);
			} catch (int err){	//Didn't have marker in previous frame, check the current frame
				try{
					coordinate initCoordinate = mainThread->markerSelector->getCoordinate(i, currentFrame);
					coordinate coordinatesReturned = getMarkerCoordinates(currentImage,i, initCoordinate, mainThread->markerSelector->markers[i].histogram);
					//mainThread->SetStatusText(wxString::Format(wxT("%s %d"),_("Returned current "), i));
					//sleep(1);
					mainThread->markerSelector->setCoordinate(i,coordinatesReturned.xCoordinate, coordinatesReturned.yCoordinate, currentFrame);
					//Digitize the marker
					mainThread->imagePanel->digitizeXY(coordinatesReturned.xCoordinate,coordinatesReturned.yCoordinate, (double) mainThread->markerSelector->markers[i].markerRadius);
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
coordinate TrackingThread::getMarkerCoordinates(wxImage currentImage,int markerIndice, coordinate coordinates, double** histogram){
	/*Go through the search area, check the closeness of each of the marker-sized histograms vs the marker histogram.
	Digitize the closest match, provided that it is above the threshold.*/
	double** markerHistogram = mainThread->markerSelector->markers[markerIndice].histogram;
	std::vector<coordinateCloseness> checkClose;
	std::vector<coordinate> samplingCoordinates = *(mainThread->markerSelector->markers[markerIndice].radiusCoordinates);
	std::vector<coordinate> searchCoordinates = *(mainThread->markerSelector->markers[markerIndice].searchCoordinates);

	for (int i = 0;i<searchCoordinates.size();++i){
		double x = coordinates.xCoordinate+searchCoordinates[i].xCoordinate;
		double y = coordinates.yCoordinate+searchCoordinates[i].yCoordinate;
		coordinate check(x,y,-1);
		//mainThread->SetStatusText(wxString::Format(wxT("%s %d"),_("searchCoordinate #"), i));
		double closeness = mainThread->markerSelector->getCloseness(markerHistogram,getHistogram(currentImage,check,samplingCoordinates));
		
		checkClose.push_back(coordinateCloseness(x,y,closeness));
	}
	
	std::sort(checkClose.begin(),checkClose.end());	//Sort the closeness values to ascending order, best closeness is the last
	coordinateCloseness bestMatch = checkClose.back();
	//mainThread->SetStatusText(wxString::Format(wxT("%s %f %s %f"),_("CheckedCloseness, max"), bestMatch.closeness,_("last"), (checkClose.front()).closeness));
	//sleep(1);
	return coordinate(bestMatch.x,bestMatch.y,-1);
}

/**Get the histogram of the current marker*/
double** TrackingThread::getHistogram(wxImage currentImage,coordinate coordinates, std::vector<coordinate> samplingCoordinates){

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
		if (xCoordinate+samplingCoordinates[i].xCoordinate >=0 && xCoordinate+samplingCoordinates[i].xCoordinate < currentImage.GetWidth()
			&& yCoordinate+samplingCoordinates[i].yCoordinate >=0 && yCoordinate+samplingCoordinates[i].yCoordinate < currentImage.GetHeight()
			){
			histogram[0][currentImage.GetRed(xCoordinate+samplingCoordinates[i].xCoordinate,yCoordinate+samplingCoordinates[i].yCoordinate)]		+= 1;
			histogram[1][currentImage.GetGreen(xCoordinate+samplingCoordinates[i].xCoordinate,yCoordinate+samplingCoordinates[i].yCoordinate)]	+= 1;
			histogram[2][currentImage.GetBlue(xCoordinate+samplingCoordinates[i].xCoordinate,yCoordinate+samplingCoordinates[i].yCoordinate)]		+= 1;
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
