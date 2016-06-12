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
#include "../markerSelector/CoordinateTracker.h"
#include "../markerSelector/CoordinateFlowTracker.h"
#include <opencv2/opencv.hpp>	//OpenCV
//#include <opencv2/video/tracking.hpp>	//calcOpticalFlowSF
#include <opencv2/optflow.hpp>	//calcOpticalFlowSF
#include <cmath>		//sqrt

TrackingThread::TrackingThread(DigitizerFrame* mainThreadIn){
	mainThread = mainThreadIn;
	cv::namedWindow("Flow");
}

void TrackingThread::startThread(){
	tThread = std::thread(&TrackingThread::run,this);
}

void TrackingThread::run(){

	int currentFrame = mainThread->slider->GetValue();
	//coordinate coordinatesReturned[mainThread->markerSelector->markers.size()];
	//mainThread->SetStatusText(wxString::Format(wxT("%s %d"),_("Thread started, frame #"), currentFrame));
	//Go through the frames in the video
	bool gotMarker;
	int markersFound;
	coordinate initCoordinate;
	//wxImage *currentImage;
	unsigned char* currentImageData;
	unsigned char* previousImageData;
	//printf("Started tracking thread\n");
	while (mainThread->trackOn == true && currentFrame <= mainThread->videoReader->getNumberOfIndices()){
		/*Make a copy of the current image*/
		//printf("Track Started\n");
		 //currentImage = new wxImage(mainThread->imagePanel->currentClearImage);
		 currentImageData = mainThread->imagePanel->currentImageData;
		 previousImageData = mainThread->imagePanel->previousImageData;
		 if (previousImageData == NULL){
		 	previousImageData = currentImageData; 
		 }
		//mainThread->SetStatusText(wxString::Format(wxT("%s %d"),_("In loop, frame #"), currentFrame));
		//Go through all of the markers in the image
		markersFound = 0;
		int keepTracking = 1;
		std::vector<std::thread> threads;
		std::vector<CoordinateTracker*> coordinateTrackers;
		for (int i = 0; i<mainThread->markerSelector->markers.size();++i){
			//Look for coordinate in the previous image/
			//printf("Marker %d\n",i);
			gotMarker = false;
			
			/*Check out whether the current or the previous frame has the marker digitized*/
			try{	
				initCoordinate = mainThread->markerSelector->getCoordinate(i, currentFrame);
				//Check if we've got a known marker color
				if (mainThread->markerSelector->markers[i].fourBitColors == NULL){
					//Get the color from the image
					mainThread->markerSelector->markers[i].fourBitColors = getColor(currentImageData, mainThread->imagePanel->imSize.x, mainThread->imagePanel->imSize.y,(int) initCoordinate.xCoordinate,(int) initCoordinate.yCoordinate);	
					//Get the histogram from the image
					mainThread->markerSelector->markers[i].histogram = getHistogram16(currentImageData, mainThread->imagePanel->imSize.x, mainThread->imagePanel->imSize.y, initCoordinate, *(mainThread->markerSelector->markers[i].radiusCoordinates));
				}
				
				//printf("Got init previous %d %d\n",(int)initCoordinate.xCoordinate,(int)initCoordinate.yCoordinate);
				gotMarker = true;
			} catch (...){	//Didn't have marker in current frame, check the previous frame
				//printf("Tried getting current frame marker, caught %d\n",err);
				try{
					initCoordinate = mainThread->markerSelector->getCoordinate(i, currentFrame-1);
					gotMarker = true;
					//printf("Got init current %d %d\n",(int)initCoordinate.xCoordinate,(int)initCoordinate.yCoordinate);
					
				} catch (...){
					//Marker has not been digitized in the previous or the current frame, so do nothing for this marker
					//printf("Tried getting previous frame marker, caught %d\n",err);
					
				}

			}
			//Digitize if we've got an init coordinate...
			if (gotMarker){
				//printf("Found marker tracking thread\n");
				//coordinate coordinatesReturned = getMarkerCoordinates(currentImage,i, initCoordinate, mainThread->markerSelector->markers[i].histogram);
				//int trackX, trackY;
				try{
					//std::vector<coordinate> areaCoordinates = getMarkerCoordinatesRegionGrow(currentImage,i, initCoordinate);
					//printf("Try to get marker coordinates region grow in tracking thread\n");
					//Implement optical flow-assisted tracking pop each marker to its own thread 
					if (true) {
					
						CoordinateFlowTracker* coordinateTracker = new CoordinateFlowTracker(currentImageData, mainThread->imagePanel->imSize.x, mainThread->imagePanel->imSize.y, i, initCoordinate, mainThread->markerSelector->markers[i].histogram, mainThread->markerSelector->markers[i].colorTolerance, mainThread->markerSelector->markers[i].radiusCoordinates,	mainThread->markerSelector->markers[i].searchCoordinates,previousImageData,mainThread->markerSelector->markers[i].fourBitColors,mainThread->markerSelector->markers[i].markerRadius);
						coordinateTrackers.push_back(coordinateTracker);
						threads.push_back(std::thread(&CoordinateTracker::getCoordinates, coordinateTrackers.back()));							
						//mainThread->imagePanel->digitizeXYArea(areaCoordinates);
					}else {
						CoordinateTracker* coordinateTracker = new CoordinateTracker(currentImageData, mainThread->imagePanel->imSize.x, mainThread->imagePanel->imSize.y, i, initCoordinate, mainThread->markerSelector->markers[i].histogram, mainThread->markerSelector->markers[i].colorTolerance, mainThread->markerSelector->markers[i].radiusCoordinates,	mainThread->markerSelector->markers[i].searchCoordinates);
						coordinateTrackers.push_back(coordinateTracker);
						threads.push_back(std::thread(&CoordinateTracker::getCoordinates, coordinateTrackers.back()));
						//Try histogram-based tracking
						//coordinate temp = getMarkerCoordinates(currentImageData, mainThread->imagePanel->imSize.x, mainThread->imagePanel->imSize.y, i, initCoordinate, mainThread->markerSelector->markers[i].histogram,mainThread->markerSelector->markers[i].colorTolerance);
						//trackX = temp.xCoordinate;
						//trackY = temp.yCoordinate;

					}
					//mainThread->markerSelector->setCoordinate(i, trackX, trackY, currentFrame);
					//printf("Set coordinate\n");
					//Digitize the marker
					
					//std::this_thread::sleep_for (std::chrono::milliseconds(100));
					//printf("Digitized coordinate\n");
					++markersFound;
				}catch (...){
					//printf("Could not digitize in tracking thread\n");
					keepTracking = 0;	//Set to stop the tracking thread
					//printf("Tried digitizing, caught %d\n",err);
					mainThread->markerSelector->currentMarker = i;
					mainThread->markerSelector->SetSelection(i);	
					//printf("Couldn't find marker in tracking thread\n");
					break;	//Stop the loop
				}
			}
		}

		//Collect the threads
		//printf("Start collecting Threads\n");
		for (int i = 0; i < threads.size(); ++i) {
			threads.at(i).join();
			if (coordinateTrackers.at(i)->foundCoord.xCoordinate > -1) {
				//printf("Found coords %d marker\n", coordinateTrackers.at(i)->markerIndice);
				mainThread->markerSelector->setCoordinate(coordinateTrackers.at(i)->markerIndice, coordinateTrackers.at(i)->foundCoord.xCoordinate, coordinateTrackers.at(i)->foundCoord.yCoordinate, currentFrame);
			}
			delete coordinateTrackers.at(i);
		}
		//printf("Frame done in tracking thread\n");
		mainThread->redrawFrame();	//Re-draw the digitized markers
		mainThread->imagePanel->reFreshImage();

		//IMPLEMENT OPTICAL FLOW HERE
		if (mainThread->previousFrame + 1 == mainThread->currentFrame) {
			/*
			mainThread->imagePanel->currentImageData;
			mainThread->imagePanel->previousImageData;
			mainThread->imagePanel->imSize.x, mainThread->imagePanel->imSize.y
			*/
			cv::Mat curr(mainThread->imagePanel->imSize.y, mainThread->imagePanel->imSize.x, CV_8UC3, mainThread->imagePanel->currentImageData);	//Mat(rows,cols);
			cv::Mat prev(mainThread->imagePanel->imSize.y, mainThread->imagePanel->imSize.x, CV_8UC3, mainThread->imagePanel->previousImageData);	//Mat(rows,cols);
			//Convert to gray scale
			//cv::Mat prevGray;
			//cv::Mat currGray;
			//cv::cvtColor(prev, prevGray, cv::COLOR_BGR2GRAY);
			//cv::cvtColor(curr, currGray, cv::COLOR_BGR2GRAY);
			cv::Mat flow;
			//cv::calcOpticalFlowFarneback(prevGray, currGray, flow, 0.5, 3, 7,3,5,1.2,0);
			cv::optflow::calcOpticalFlowSF(prev, curr,flow,3, 2, 4, 4.1, 25.5, 18, 55.0, 25.5, 0.35, 18, 55.0, 25.5, 10);
			//Convert flow to image
			cv::Mat visual(curr.rows, curr.cols, CV_8UC1);
			//Set the values of visual
			double maxVal=0;
			double movementMagnitude;
			unsigned char movementVal;
			double denom = 30;
			for (int r = 0; r < flow.rows; ++r) {
				for (int c = 0; c < flow.cols; ++c) {
					const cv::Point2f& val = flow.at<cv::Point2f>(r, c);
					double movementMagnitude = std::sqrt(val.x * val.x + val.y * val.y);
					movementVal = (movementMagnitude / denom)*255.0 > 255.0 ? (unsigned char)255 : (unsigned char)((movementMagnitude / denom)*255.0);
					visual.at<unsigned char>(r, c) = movementVal;
					if (movementMagnitude > maxVal) {
						maxVal = movementMagnitude;
					}
				}
			}
			printf("Max movement %f\n", maxVal);
			imshow("Flow", visual);

		}


		//Advance frame if at least one marker was digitized
		if (markersFound > 0 && keepTracking == 1){
			/*Proceed to the next frame, if this was not the last frame*/
			if (currentFrame < mainThread->videoReader->getNumberOfIndices()){
				//std::this_thread::sleep_for (std::chrono::milliseconds(20));
				mainThread->previousFrame = currentFrame;
				++currentFrame;
				//printf("Get next frame in tracking thread\n");
				mainThread->slider->SetValue(currentFrame);
				mainThread->videoReader->readFrameFromDisk(currentFrame);
				mainThread->imagePanel->setImage(mainThread->videoReader->width,mainThread->videoReader->height,mainThread->videoReader->decodedFrame,true,false);
				mainThread->currentFrame = currentFrame;
			}else{
				/*Stop the thread*/
				mainThread->toggleTrack->SetValue(false);	/*Set the track on toggle to off*/
				mainThread->trackOn = false;	/*Stop tracking*/
				//printf("Trying to stop the thread\n");
				//delete currentImage; /*Try to save mem...*/
				break;
			}
			
		}else{
			/*Stop the thread*/
			mainThread->toggleTrack->SetValue(false);	/*Set the track on toggle to off*/
			mainThread->trackOn = false;	/*Stop tracking*/
		}
		//delete currentImage; /*Try to save mem...*/
	}
	//printf("Exiting thread\n");
	mainThread->toggleTrack->SetValue(false);	/*Set the track on toggle to off*/
	mainThread->trackOn = false;	/*Stop tracking*/
	mainThread->printCoordinates();	/*Update the coordinate list*/
	/*Done with automatic digitizing*/
}

/**Look for the marker in the image*/
/*
coordinate TrackingThread::getMarkerCoordinates(wxImage *currentImage,int markerIndice, coordinate coordinates, double** histogram){
	//Go through the search area, check the closeness of each of the marker-sized histograms vs the marker histogram.
	//Digitize the closest match, provided that it is above the threshold.
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
	printf("Best closeness %f\n", bestMatch.closeness);
	//mainThread->SetStatusText(wxString::Format(wxT("%s %f %s %f"),_("CheckedCloseness, max"), bestMatch.closeness,_("last"), (checkClose.front()).closeness));
	//sleep(1);
	return coordinate(bestMatch.x,bestMatch.y,-1);
}
*/
/**Look for the marker in the image*/
coordinate TrackingThread::getMarkerCoordinates(unsigned char *currentImage, int width, int height, int markerIndice, coordinate coordinates, Histogram* histogram,int colorTolerance, std::vector<coordinate> *samplingCoordinates, std::vector<coordinate> *searchCoordinates) throw(int) {
	/*Go through the search area, check the closeness of each of the marker-sized histograms vs the marker histogram.
	Digitize the closest match, provided that it is above the threshold.*/
	std::vector<coordinateCloseness> checkClose;
	double xx = round(coordinates.xCoordinate);
	double yy = round(coordinates.yCoordinate);
	for (int i = 0; i<searchCoordinates->size(); ++i) {
		double x = xx + (*searchCoordinates).at(i).xCoordinate;
		double y = yy + (*searchCoordinates).at(i).yCoordinate;
		coordinate check(x, y, -1);
		//mainThread->SetStatusText(wxString::Format(wxT("%s %d"),_("searchCoordinate #"), i));
		Histogram *temp = getHistogram16(currentImage, width, height, check, *samplingCoordinates);
		double closeness = MarkerSelector::getCloseness16(histogram, temp);
		delete temp;
		//printf("i %d x0 %f y0 %f x %f y %f close %f\n",i, coordinates.xCoordinate, coordinates.yCoordinate, x, y, closeness);
		checkClose.push_back(coordinateCloseness(x, y, closeness));
	}

	std::sort(checkClose.begin(), checkClose.end());	//Sort the closeness values to ascending order, best closeness is the last
	coordinateCloseness bestMatch = checkClose.back();
	//printf("Best closeness %f %f %f\n", bestMatch.closeness, bestMatch.x, bestMatch.y);
	//mainThread->SetStatusText(wxString::Format(wxT("%s %f %s %f"),_("CheckedCloseness, max"), bestMatch.closeness,_("last"), (checkClose.front()).closeness));
	//sleep(1);
	if (bestMatch.closeness >= ((double)colorTolerance)/256.0*3.0){
		return coordinate(bestMatch.x, bestMatch.y, -1);
	}else{
		throw 4;
	}
}

/*Calculate marker optical flow*/
coordinate TrackingThread::getFlow(unsigned char *prevImage,unsigned char *currentImage, int width, int height,coordinate coordinates,std::vector<coordinate> *searchCoordinates){
	return coordinate(coordinates.xCoordinate,coordinates.yCoordinate,-1);	//Dummy function
}


/*Calculate region grow*/
std::vector<coordinate> TrackingThread::getMarkerCoordinatesRegionGrow(unsigned char *currentImage, int width, int height, coordinate startSearch,unsigned char *markerColor, int colorTolerance,std::vector<coordinate> *searchCoordinates,double markerRadius) throw(int){
//printf("getMCoords getWidth\n");
	int imageWidth =width;
	int imageHeight =height;
	//printf("getMCoords goThroughSearchCoordinates %d\n",(int) searchCoordinates.size());
	for (int i = 0;i<(*searchCoordinates).size();++i){
		double x = startSearch.xCoordinate+(*searchCoordinates)[i].xCoordinate;
		double y = startSearch.yCoordinate+(*searchCoordinates)[i].yCoordinate;
		/*Check whether color matches, if it does, grow region to confirm that the marker is big enough*/
		unsigned char* pixelColor = getColor(currentImage,imageWidth,imageHeight,(int) x, (int) y);
		//printf("getMCoords gotColor %d %d\n",(int) x, (int) y);
		if(	/*If color match is close enough, do region growing*/
			(((int)pixelColor[0])-((int)markerColor[0])) < colorTolerance && (((int)pixelColor[0])-((int)markerColor[0])) > -colorTolerance &&
			(((int)pixelColor[1])-((int)markerColor[1])) < colorTolerance && (((int)pixelColor[1])-((int)markerColor[1])) > -colorTolerance &&
			(((int)pixelColor[2])-((int)markerColor[2])) < colorTolerance && (((int)pixelColor[2])-((int)markerColor[2])) > -colorTolerance			
			)
		{
			std::vector<coordinate> areaCoordinates = growRegion(currentImage,imageWidth,imageHeight,x,y,markerColor, colorTolerance);
			//printf("getMCoords grewArea %d %d\n",(int) x, (int) y);
			if (areaCoordinates.size() >= 5){// M_PI*(markerRadius)*(markerRadius)/4){	//At least the size of a circle of 1/2 radius of the marker
				return areaCoordinates;								/*First sufficiently large marker returned... */
				
			}
		}
	}
	//printf("getMCoords All done\n");
	throw 2;	/*No marker found, throw error. Implement missing marker at some point...*/
	//return coordinates;	/*No marker found, shouldn't get here*/
}



/**Look for the marker in the image based on region growing*/
std::vector<coordinate> TrackingThread::getMarkerCoordinatesRegionGrow(unsigned char *currentImage,int markerIndice, coordinate coordinates) throw(int){
	//printf("getMCoords getWidth\n");
	int imageWidth =mainThread->imagePanel->imSize.x;
	int imageHeight =mainThread->imagePanel->imSize.y;
	//printf("getMCoords gotWidth\n");
	/*Go through the search area, return the first continuous marker area found.*/
	unsigned char* markerColor = mainThread->markerSelector->markers[markerIndice].fourBitColors;
	//printf("getMCoords gotMColor\n");
		
	int colorTolerance = mainThread->markerSelector->markers[markerIndice].colorTolerance;
	std::vector<coordinate> searchCoordinates = *(mainThread->markerSelector->markers[markerIndice].searchCoordinates);
	double markerRadius =  (double) (mainThread->markerSelector->markers[markerIndice].markerRadius);
	//printf("getMCoords goThroughSearchCoordinates %d\n",(int) searchCoordinates.size());
	for (int i = 0;i<searchCoordinates.size();++i){
		double x = coordinates.xCoordinate+searchCoordinates[i].xCoordinate;
		double y = coordinates.yCoordinate+searchCoordinates[i].yCoordinate;
		/*Check whether color matches, if it does, grow region to confirm that the marker is big enough*/
		//printf("getMCoords getColor %d %d %d %d\n",imageWidth,imageHeight,(int) x, (int) y);
		unsigned char* pixelColor = getColor(currentImage,imageWidth,imageHeight,(int) x, (int) y);
		//printf("getMCoords gotColor %d %d\n",(int) x, (int) y);
		if(	/*If color match is close enough, do region growing*/
			(((int)pixelColor[0])-((int)markerColor[0])) < colorTolerance && (((int)pixelColor[0])-((int)markerColor[0])) > -colorTolerance &&
			(((int)pixelColor[1])-((int)markerColor[1])) < colorTolerance && (((int)pixelColor[1])-((int)markerColor[1])) > -colorTolerance &&
			(((int)pixelColor[2])-((int)markerColor[2])) < colorTolerance && (((int)pixelColor[2])-((int)markerColor[2])) > -colorTolerance			
			)
		{
			//printf("Marker %d %d %d pixel %d %d %d\n",markerColor[0],markerColor[1],markerColor[2],pixelColor[0],pixelColor[1],pixelColor[2]);
			//printf("getMCoords growArea %d %d\n",(int) x, (int) y);
			std::vector<coordinate> areaCoordinates = growRegion(currentImage,imageWidth,imageHeight,x,y,markerColor, colorTolerance);
			//printf("getMCoords grewArea %d %d\n",(int) x, (int) y);
			if (areaCoordinates.size() >= 5){// M_PI*(markerRadius)*(markerRadius)/4){	//At least the size of a circle of 1/2 radius of the marker
				return areaCoordinates;								/*First sufficiently large marker returned... */
				
			}
		}
	}
	//printf("getMCoords All done\n");
	throw 2;	/*No marker found, throw error. Implement missing marker at some point...*/
	//return coordinates;	/*No marker found, shouldn't get here*/
}

/**Region grow*/
std::vector<coordinate> TrackingThread::growRegion(unsigned char *currentImage,int imageWidth,int imageHeight,double x, double y, unsigned char* markerColor, int colorTolerance){
		int neighbourhoodSize = 4;
	int ** neighbourhood = new int*[neighbourhoodSize];
	for (int i = 0; i<4;++i){
		neighbourhood[i]=new int[2];
	}
	
	char* visited = new char[imageWidth*imageHeight]();	/*ReserveMemory and init to zero*/
	//visited[(int) (x+imageWidth*y)] =1;	/*Set the init pixel to 1*/
	int regionPixels = 0;
	std::vector<coordinate> pixelQueue;
	std::vector<coordinate> areaCoordinates;
	pixelQueue.push_back(coordinate(x,y,-1));
	coordinate nextPixel;
	while (pixelQueue.size() > 0){ //Go through all cells in queue
		nextPixel = pixelQueue.front();        /*Get the pixel with the lowest cost and remove it from the queue*/
		pixelQueue.erase(pixelQueue.begin());	/*remove the pixel from the queue*/
		/*In case the pixel has been visited subsequent to having been added*/
		while (visited[(int) (nextPixel.xCoordinate+imageWidth*nextPixel.yCoordinate)] == 1 && pixelQueue.size() > 0){
			nextPixel = pixelQueue.front();        /*Get the pixel with the lowest cost and remove it from the queue*/
			pixelQueue.erase(pixelQueue.begin());	/*remove the pixel from the queue*/
		}
		visited[(int) (nextPixel.xCoordinate+imageWidth*nextPixel.yCoordinate)] = 1;
		areaCoordinates.push_back(nextPixel);
		++regionPixels;        //Add the new pixel to the area
		/*        Add 4-connected neighbourhood to the queue, unless the
		neighbourhood pixels have already been visited or are part of the
		mask already*/
		neighbourhood[0][0] = (int) nextPixel.xCoordinate-1;        /*Left one*/
		neighbourhood[1][0] = (int) nextPixel.xCoordinate+1;        /*Right one*/
		neighbourhood[2][0] = (int) nextPixel.xCoordinate;
		neighbourhood[3][0] = (int) nextPixel.xCoordinate;
		
		neighbourhood[0][1] = (int) nextPixel.yCoordinate;
		neighbourhood[1][1] = (int) nextPixel.yCoordinate;
		neighbourhood[2][1] = (int) nextPixel.yCoordinate-1;        /*Up one*/
		neighbourhood[3][1] = (int) nextPixel.yCoordinate+1;        /*Down one*/

		//checkNeighbours and add to queue;
		int* coordinates;
        for (int r = 0;r<neighbourhoodSize;++r){
			coordinates = neighbourhood[r];
			
			//Check that the neighbour is within the image.
            if (coordinates[0] >= 0 && coordinates[0] < imageWidth && coordinates[1] >=0 && coordinates[1] < imageHeight){ 
				//Add to queue if the neighbour has not been visited, and is of marker color
				unsigned char* pixelColor =  getColor(currentImage,imageWidth,imageHeight,coordinates[0], coordinates[1]);
				if (visited[coordinates[0]+coordinates[1]*imageWidth] == (unsigned char) 0 && 
					(((int)pixelColor[0])-((int)markerColor[0])) < colorTolerance && (((int)pixelColor[0])-((int)markerColor[0])) > -colorTolerance &&
					(((int)pixelColor[1])-((int)markerColor[1])) < colorTolerance && (((int)pixelColor[1])-((int)markerColor[1])) > -colorTolerance &&
					(((int)pixelColor[2])-((int)markerColor[2])) < colorTolerance && (((int)pixelColor[2])-((int)markerColor[2])) > -colorTolerance			
					)
				{
					pixelQueue.push_back(coordinate((double) coordinates[0],(double)coordinates[1],-1));
				}
				delete pixelColor;	//Try to save memory..
            }
        }

		
	}
	/*Release memory*/
	delete visited;
	for (int i = 0; i<4;++i){
		delete neighbourhood[i];
	}
	delete neighbourhood;
	
	return areaCoordinates;
}



/**Look for the marker in the image based on region growing*/
std::vector<coordinate> TrackingThread::getMarkerCoordinatesRegionGrow(wxImage *currentImage,int markerIndice, coordinate coordinates) throw(int){
	/*Go through the search area, return the first continuous marker area found.*/
	unsigned char* markerColor = mainThread->markerSelector->markers[markerIndice].fourBitColors;
	int colorTolerance = mainThread->markerSelector->markers[markerIndice].colorTolerance;
	std::vector<coordinate> searchCoordinates = *(mainThread->markerSelector->markers[markerIndice].searchCoordinates);
	double markerRadius =  (double) (mainThread->markerSelector->markers[markerIndice].markerRadius);
	for (int i = 0;i<searchCoordinates.size();++i){
		double x = coordinates.xCoordinate+searchCoordinates[i].xCoordinate;
		double y = coordinates.yCoordinate+searchCoordinates[i].yCoordinate;
		/*Check whether color matches, if it does, grow region to confirm that the marker is big enough*/
		unsigned char* pixelColor = getColor(currentImage,(int) x, (int) y);
		
		if(	/*If color match is close enough, do region growing*/
			(((int)pixelColor[0])-((int)markerColor[0])) < colorTolerance && (((int)pixelColor[0])-((int)markerColor[0])) > -colorTolerance &&
			(((int)pixelColor[1])-((int)markerColor[1])) < colorTolerance && (((int)pixelColor[1])-((int)markerColor[1])) > -colorTolerance &&
			(((int)pixelColor[2])-((int)markerColor[2])) < colorTolerance && (((int)pixelColor[2])-((int)markerColor[2])) > -colorTolerance			
			)
		{
			//printf("Marker %d %d %d pixel %d %d %d\n",markerColor[0],markerColor[1],markerColor[2],pixelColor[0],pixelColor[1],pixelColor[2]);
			std::vector<coordinate> areaCoordinates = growRegion(currentImage,x,y,markerColor, colorTolerance);
			if (areaCoordinates.size() >= 5){// M_PI*(markerRadius)*(markerRadius)/4){	//At least the size of a circle of 1/2 radius of the marker
				return areaCoordinates;								/*First sufficiently large marker returned... */
				
			}
		}
	}
	throw 2;	/*No marker found, throw error. Implement missing marker at some point...*/
	//return coordinates;	/*No marker found, shouldn't get here*/
}

std::vector<coordinate> TrackingThread::growRegion(wxImage *currentImage,double x, double y, unsigned char* markerColor, int colorTolerance){
	int imageWidth =currentImage->GetWidth(); 
	int imageHeight =currentImage->GetHeight();
	int neighbourhoodSize = 4;
	int ** neighbourhood = new int*[neighbourhoodSize];
	for (int i = 0; i<4;++i){
		neighbourhood[i]=new int[2];
	}
	
	char* visited = new char[imageWidth*imageHeight]();	/*ReserveMemory and init to zero*/
	//visited[(int) (x+imageWidth*y)] =1;	/*Set the init pixel to 1*/
	int regionPixels = 0;
	std::vector<coordinate> pixelQueue;
	std::vector<coordinate> areaCoordinates;
	pixelQueue.push_back(coordinate(x,y,-1));
	coordinate nextPixel;
	while (pixelQueue.size() > 0){ //Go through all cells in queue
		nextPixel = pixelQueue.front();        /*Get the pixel with the lowest cost and remove it from the queue*/
		pixelQueue.erase(pixelQueue.begin());	/*remove the pixel from the queue*/
		/*In case the pixel has been visited subsequent to having been added*/
		while (visited[(int) (nextPixel.xCoordinate+imageWidth*nextPixel.yCoordinate)] == 1 && pixelQueue.size() > 0){
			nextPixel = pixelQueue.front();        /*Get the pixel with the lowest cost and remove it from the queue*/
			pixelQueue.erase(pixelQueue.begin());	/*remove the pixel from the queue*/
		}
		visited[(int) (nextPixel.xCoordinate+imageWidth*nextPixel.yCoordinate)] = 1;
		areaCoordinates.push_back(nextPixel);
		++regionPixels;        //Add the new pixel to the area
		/*        Add 4-connected neighbourhood to the queue, unless the
		neighbourhood pixels have already been visited or are part of the
		mask already*/
		neighbourhood[0][0] = (int) nextPixel.xCoordinate-1;        /*Left one*/
		neighbourhood[1][0] = (int) nextPixel.xCoordinate+1;        /*Right one*/
		neighbourhood[2][0] = (int) nextPixel.xCoordinate;
		neighbourhood[3][0] = (int) nextPixel.xCoordinate;
		
		neighbourhood[0][1] = (int) nextPixel.yCoordinate;
		neighbourhood[1][1] = (int) nextPixel.yCoordinate;
		neighbourhood[2][1] = (int) nextPixel.yCoordinate-1;        /*Up one*/
		neighbourhood[3][1] = (int) nextPixel.yCoordinate+1;        /*Down one*/

		//checkNeighbours and add to queue;
		int* coordinates;
        for (int r = 0;r<neighbourhoodSize;++r){
			coordinates = neighbourhood[r];
			
			//Check that the neighbour is within the image.
            if (coordinates[0] >= 0 && coordinates[0] < imageWidth && coordinates[1] >=0 && coordinates[1] < imageHeight){ 
				//Add to queue if the neighbour has not been visited, and is of marker color
				unsigned char* pixelColor =  getColor(currentImage,coordinates[0], coordinates[1]);
				if (visited[coordinates[0]+coordinates[1]*imageWidth] == (unsigned char) 0 && 
					(((int)pixelColor[0])-((int)markerColor[0])) < colorTolerance && (((int)pixelColor[0])-((int)markerColor[0])) > -colorTolerance &&
					(((int)pixelColor[1])-((int)markerColor[1])) < colorTolerance && (((int)pixelColor[1])-((int)markerColor[1])) > -colorTolerance &&
					(((int)pixelColor[2])-((int)markerColor[2])) < colorTolerance && (((int)pixelColor[2])-((int)markerColor[2])) > -colorTolerance			
					)
				{
					pixelQueue.push_back(coordinate((double) coordinates[0],(double)coordinates[1],-1));
				}
				delete pixelColor;	//Try to save memory..
            }
        }

		
	}
	/*Release memory*/
	delete visited;
	for (int i = 0; i<4;++i){
		delete neighbourhood[i];
	}
	delete neighbourhood;
	
	return areaCoordinates;
}


/**Get the histogram of the current marker*/
/*
double** TrackingThread::getHistogram(unsigned char *currentImage, int width, int height, coordinate coordinates, std::vector<coordinate> samplingCoordinates) {

	//Get the histogram
	double** histogram;
	histogram = new double*[3];	//Color figure comprises 3 different colors...
	for (int i = 0; i<3; ++i) {
		histogram[i] = new double[256](); //256 possible intensities of a given color, the () initialises the values to zero
	}
	int xCoordinate = coordinates.xCoordinate;
	int yCoordinate = coordinates.yCoordinate;
	//get the colorvalues for the histograms
	for (int i = 0; i<samplingCoordinates.size(); ++i) {
		if (xCoordinate + samplingCoordinates[i].xCoordinate >= 0 && xCoordinate + samplingCoordinates[i].xCoordinate < width
			&& yCoordinate + samplingCoordinates[i].yCoordinate >= 0 && yCoordinate + samplingCoordinates[i].yCoordinate < height
			) {
			histogram[0][(int) currentImage[(int) ((xCoordinate + samplingCoordinates[i].xCoordinate)*3 + (yCoordinate + samplingCoordinates[i].yCoordinate)*width*3 + 0)]] += 1;
			histogram[1][(int) currentImage[(int) ((xCoordinate + samplingCoordinates[i].xCoordinate)*3 + (yCoordinate + samplingCoordinates[i].yCoordinate)*width*3 + 1)]] += 1;
			histogram[2][(int) currentImage[(int) ((xCoordinate + samplingCoordinates[i].xCoordinate)*3 + (yCoordinate + samplingCoordinates[i].yCoordinate)*width*3 + 2)]] += 1;
		}
	}
	//Normalize sum to 1 (maximum, next to border sum of histogram will be less than 1
	for (int j = 0; j<3; ++j) {
		for (int i = 0; i<256; ++i) {
			histogram[j][i] /= ((double)samplingCoordinates.size());
		}
	}
	return histogram;
}
*/

/**Get the histogram of the current marker
 * The histograms are reduced to 16 levels per color channel
 * */
Histogram* TrackingThread::getHistogram16(unsigned char *currentImage, int width, int height, coordinate coordinates, std::vector<coordinate> samplingCoordinates) {
	int bins = 16;
	/*Get the histogram*/
	Histogram *histogram = new Histogram(3,bins);
	int xCoordinate = coordinates.xCoordinate;
	int yCoordinate = coordinates.yCoordinate;
	/*get the colorvalues for the histograms*/
	for (int i = 0; i<samplingCoordinates.size(); ++i) {
		if (xCoordinate + samplingCoordinates[i].xCoordinate >= 0 && xCoordinate + samplingCoordinates[i].xCoordinate < width
			&& yCoordinate + samplingCoordinates[i].yCoordinate >= 0 && yCoordinate + samplingCoordinates[i].yCoordinate < height
			) {
			histogram->histogram[0][((int) currentImage[(int) ((xCoordinate + samplingCoordinates[i].xCoordinate)*3 + (yCoordinate + samplingCoordinates[i].yCoordinate)*width*3 + 0)])>>4] += 1;
			histogram->histogram[1][((int) currentImage[(int)((xCoordinate + samplingCoordinates[i].xCoordinate)*3 + (yCoordinate + samplingCoordinates[i].yCoordinate)*width*3 + 1)])>>4] += 1;
			histogram->histogram[2][((int) currentImage[(int)((xCoordinate + samplingCoordinates[i].xCoordinate)*3 + (yCoordinate + samplingCoordinates[i].yCoordinate)*width*3 + 2)])>>4] += 1;
		}
	}
	/*Normalize sum to 1 (maximum, next to border sum of histogram will be less than 1*/
	for (int j = 0; j<3; ++j) {
		for (int i = 0; i<bins; ++i) {
			histogram->histogram[j][i] /= ((double)samplingCoordinates.size());
		}
	}
	return histogram;
}

/**Get the histogram of the current marker*/
/*
double** TrackingThread::getHistogram(wxImage *currentImage,coordinate coordinates, std::vector<coordinate> samplingCoordinates){

	//Get the histogram
	double** histogram;
	histogram = new double*[3];	//Color figure comprises 3 different colors...
	for (int i = 0; i<3;++i){
		histogram[i] = new double[256](); //256 possible intensities of a given color, the () initialises the values to zero
	}
	int xCoordinate = coordinates.xCoordinate;
	int yCoordinate = coordinates.yCoordinate;
	//get the colorvalues for the histograms
	for (int i = 0; i<samplingCoordinates.size(); ++i){
		if (xCoordinate+samplingCoordinates[i].xCoordinate >=0 && xCoordinate+samplingCoordinates[i].xCoordinate < currentImage->GetWidth()
			&& yCoordinate+samplingCoordinates[i].yCoordinate >=0 && yCoordinate+samplingCoordinates[i].yCoordinate < currentImage->GetHeight()
			){
			histogram[0][currentImage->GetRed(xCoordinate+samplingCoordinates[i].xCoordinate,yCoordinate+samplingCoordinates[i].yCoordinate)]		+= 1;
			histogram[1][currentImage->GetGreen(xCoordinate+samplingCoordinates[i].xCoordinate,yCoordinate+samplingCoordinates[i].yCoordinate)]	+= 1;
			histogram[2][currentImage->GetBlue(xCoordinate+samplingCoordinates[i].xCoordinate,yCoordinate+samplingCoordinates[i].yCoordinate)]		+= 1;
		}
	}
	//Normalize sum to 1 (maximum, next to border sum of histogram will be less than 0
	for (int j = 0;j<3;++j){
		for (int i = 0;i<256;++i){
			histogram[j][i] /= ((double) samplingCoordinates.size());
		}
	}
	return histogram;
}	
*/

/**Get the color of the current marker*/
unsigned char* TrackingThread::getColor(wxImage *currentImage,int xCoordinate,int yCoordinate){
	/*Get the histogram*/
	unsigned char* colour;
	colour = new unsigned char[3];	/*Color figure comprises 3 different colors...*/
	/*get the color values*/
	colour[0] = currentImage->GetRed(xCoordinate,yCoordinate);
	colour[1] = currentImage->GetGreen(xCoordinate,yCoordinate);
	colour[2] = currentImage->GetBlue(xCoordinate,yCoordinate);
	return colour;
}	

/**Get the color of the current marker*/
unsigned char* TrackingThread::getColor(unsigned char *currentImage, int width, int height,int xCoordinate,int yCoordinate){
	/*Get the histogram*/
	//printf("trying to get color %d %d %d %d\n",width,height,xCoordinate,yCoordinate);
	unsigned char* colour;
	colour = new unsigned char[3];	/*Color figure comprises 3 different colors...*/
	/*get the color values*/
	colour[0] = currentImage[yCoordinate*width*3+xCoordinate*3];		//R
	colour[1] = currentImage[yCoordinate*width*3+xCoordinate*3+1];	//G
	colour[2] = currentImage[yCoordinate*width*3+xCoordinate*3+2];	//B
	return colour;
}	

