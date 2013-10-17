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
	int currentFrame = mainThread->currentFrame;
	
	coordinate coordinatesReturned[mainThread->markerSelector->markers.size()];
	//Go through the frames in the video
		//Go through all of the markers in the image
		for (int i = 0; i<mainThread->markerSelector->markers.size();++i){
			//Look for coordinate in the previous image/
			coordinatesReturned[i] = coordinate(0,0,0);
			try{
				coordinate initCoordinate = mainThread->markerSelector->getCoordinate(i, currentFrame-1);
				getMarkerCoordinates(currentImage,i,coordinatesReturned, initCoordinate, mainThread->markerSelector->markers[i].histogram);
				//mainThread->markerSelector->setCoordinate(i,coordinatesReturned[i].xCoordinate, coordinatesReturned[i].yCoordinate, currentFrame);
				//getMarkerCoordinates(currentImage,i, mainThread->markerSelector->markers[i].histogram);
			} catch (int err){	//Didn't have marker in previous frame, check the current frame
				try{
					coordinate initCoordinate = mainThread->markerSelector->getCoordinate(i, currentFrame);
					getMarkerCoordinates(currentImage,i,coordinatesReturned, initCoordinate, mainThread->markerSelector->markers[i].histogram);
					//mainThread->markerSelector->setCoordinate(i,coordinatesReturned[i].xCoordinate, coordinatesReturned[i].yCoordinate, currentFrame);
					//getMarkerCoordinates(currentImage,i, mainThread->markerSelector->markers[i].histogram);
				} catch (int err){
					//Marker has not been digitized in the previous or the current frame, so do nothing for this marker
				}

			}
		}
		//Advance frame
		currentFrame++;
	
	/*
	for (int i = 0; i<5;++i){
		mainThread->SetStatusText(wxString::Format(wxT("%s %d"),_("TrackingRunning #"), i));
		//printf("TrackingRunning %d\n",i);
		//fflush(stdout);
		sleep(1);
	}
	*/
}

/**Look for the marker in the image*/
void TrackingThread::getMarkerCoordinates(wxImage currentImage,int markerIndice,coordinate* returnCoordinate, coordinate coordinates, double** histogram){
	mainThread->SetStatusText(wxString::Format(wxT("%s %d"),_("looking for marker coordinate marker #"), markerIndice));
	sleep(1);
}

void TrackingThread::getMarkerCoordinates(wxImage currentImage,int markerIndice, double** histogram){
	mainThread->SetStatusText(wxString::Format(wxT("%s %d"),_("looking for marker coordinate marker #"), markerIndice));
	sleep(1);
}