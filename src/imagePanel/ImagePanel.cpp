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

#ifndef IMAGEPANEL_H
	#include "ImagePanel.h"
#endif
#include <cstring>	//memcpy

ImagePanel::ImagePanel(wxFrame* parent,wxWindowID id, wxString file, wxBitmapType format, const wxPoint& pos, const wxSize& sizeIn) :
wxPanel(parent,id,pos,sizeIn)
{
	
    // load the file... ideally add a check to see if loading was successful
    size = sizeIn;
    wxImage::AddHandler(new wxJPEGHandler);
    imageOrig.LoadFile(file, format);
    imageCopy = imageOrig.ConvertToImage();
    scaleFactor = (double)imageOrig.GetWidth()/(double)size.GetWidth() > (double)imageOrig.GetHeight()/(double)size.GetHeight() ? (double)imageOrig.GetWidth()/(double)size.GetWidth() : (double)imageOrig.GetHeight()/(double)size.GetHeight();
    if (scaleFactor != 1){ //Scale to fit the panel
    	imageCopy.Rescale(imageOrig.GetWidth()/scaleFactor,imageOrig.GetHeight()/scaleFactor,wxIMAGE_QUALITY_HIGH);
    	resizedImage = wxBitmap(imageCopy);//*(new wxBitmap(imageCopy));
    }else{ //No scaling needed
    	resizedImage = wxBitmap(imageOrig);//*(new wxBitmap(imageOrig));
    }
	currentImage = resizedImage.ConvertToImage();
	currentClearImage = currentImage;    
	oldSize = wxSize(resizedImage.GetWidth(),resizedImage.GetHeight());
}

ImagePanel::ImagePanel(wxFrame* parent,wxWindowID id, const wxPoint& pos, const wxSize& sizeIn) :
wxPanel(parent,id,pos,sizeIn)
{
    size = sizeIn;
	scaleFactor = 1;
	oldSize = wxSize(0,0);
	setImage(size.GetWidth(),size.GetHeight(),new unsigned char[3*size.GetWidth()*size.GetHeight()],true);
}


/*Set image to show*/
void ImagePanel::setImage(int width, int height, unsigned char* data,bool static_data, bool refresh){
	imageCopy =wxImage(width,height,data,static_data);//*(new wxImage(width,height,data,static_data));
	//imageCopy =wxImage(imageOrig);//*(new wxImage(width,height,data,static_data));
	imSize = wxSize(width,height);	/*Save the current image size*/
	//Copy current image to previous image
	if (currentImageData != NULL) {
		if (previousImageData == NULL) {
			previousImageData = new  unsigned char[imSize.x*imSize.y * 3]; /*Reserve memory on the first copy*/
		}
		std::memcpy(previousImageData, currentImageData, sizeof(unsigned char)*imSize.x*imSize.y * 3);	//Copy the data
	}
	else {
		currentImageData = new unsigned char[imSize.x*imSize.y * 3];	/*Reserve memory to copy the data*/
	}

	//delete currentImageData;		/*Should be safe on a null pointer*/
	
	/*Note, current image data is copied, so it will change when next frame is read! Image data is RGB*/
	/*
	for (int i = 0;i<width*height*3;++i){
		currentImageData[i] = data[i];
	}
	*/
	std::memcpy(currentImageData, data, sizeof(unsigned char)*imSize.x*imSize.y * 3);	//Copy the data

	double tempSF = (double)width/(double)size.GetWidth() > (double)height/(double)size.GetHeight() ? (double)width/(double)size.GetWidth() : (double)height/(double)size.GetHeight();
	/*If image is larger than image panel (i.e. tempSF > 1), scale the image to fit on screen*/
	scaleFactor = tempSF > scaleFactor ? tempSF : scaleFactor;
	/*Scale only if needed*/
    if (scaleFactor != 1){ //Scale to fit the panel
    	imageCopy.Rescale(width/scaleFactor,height/scaleFactor,wxIMAGE_QUALITY_HIGH);
    	resizedImage = wxBitmap(imageCopy);//*(new wxBitmap(imageCopy));
		if (oldSize.x != width && oldSize.y != height){
			tempImage = resizedImage;
			oldSize = imSize;
		}
    }else{ //No scaling needed
    	resizedImage = wxBitmap(imageCopy);//*(new wxBitmap(imageOrig));
		if (oldSize.x != width && oldSize.y != height){
			tempImage = resizedImage;
			oldSize = imSize;
		}

    }
	currentImage = resizedImage.ConvertToImage();
	currentClearImage = currentImage;
	
	if (refresh){
		reFreshImage();
	}
}
 
 /**Draw a circle in the current image to highlight a digitized marker*/
 void ImagePanel::digitizeXY(int xCoordinate,int yCoordinate, double radius){
 
 	lockThread.lock();
	//update the coordinates to scaled image coordinates
	xCoordinate = xCoordinate/scaleFactor;
	yCoordinate = yCoordinate/scaleFactor;
	
	//draw to the image
	double xAdd;
	double yAdd;
	for  (int i = 0; i<(int)ceil(2.0*M_PI*radius);++i){
		xAdd = radius*cos(((double)i)/(radius));
		yAdd = radius*sin(((double)i)/(radius));
		currentImage.SetRGB(xCoordinate+(int)xAdd,yCoordinate+(int)yAdd,(unsigned char) 255,(unsigned char) 0,(unsigned char) 0);
	}
	resizedImage = wxBitmap(currentImage);//*(new wxBitmap(currentImage));
	lockThread.unlock();
} 

/**Draw a grown area in the current image to highlight a digitized marker*/
void ImagePanel::digitizeXYArea(std::vector<coordinate> areaCoordinates){
	lockThread.lock();
	/*draw to the image*/
	for  (int i = 0; i<areaCoordinates.size();++i){
		currentImage.SetRGB((int) (areaCoordinates[i].xCoordinate/scaleFactor),(int) (areaCoordinates[i].yCoordinate/scaleFactor),(unsigned char) 255,(unsigned char) 0,(unsigned char) 0);
	}
	resizedImage = wxBitmap(currentImage);//*(new wxBitmap(currentImage));
	
	lockThread.unlock();
} 

void ImagePanel::reFreshImage(){
	lockThread.lock();
	wxRect tempRect =wxRect(0,0,imSize.x,imSize.y);
	Refresh(false,&tempRect);
	lockThread.unlock();
}

/**Get the histogram of the current marker*/
double** ImagePanel::getHistogram(int xCoordinate,int yCoordinate, std::vector<coordinate> *samplingCoordinates){

	/*Get the histogram*/
	double** histogram;
	histogram = new double*[3];	/*Color figure comprises 3 different colors...*/
	for (int i = 0; i<3;++i){
		histogram[i] = new double[256](); /*256 possible intensities of a given color, the () initialises the values to zero*/
	}
	/*get the colorvalues for the histograms*/
	for (int i = 0; i<samplingCoordinates->size(); ++i){
		if (xCoordinate+(*(samplingCoordinates))[i].xCoordinate >=0 && xCoordinate+(*(samplingCoordinates))[i].xCoordinate < size.GetWidth()
			&& yCoordinate+(*(samplingCoordinates))[i].yCoordinate >=0 && yCoordinate+(*(samplingCoordinates))[i].yCoordinate < size.GetHeight()
			){
			histogram[0][currentImage.GetRed(xCoordinate+(*(samplingCoordinates))[i].xCoordinate,yCoordinate+(*(samplingCoordinates))[i].yCoordinate)]		+= 1;
			histogram[1][currentImage.GetGreen(xCoordinate+(*(samplingCoordinates))[i].xCoordinate,yCoordinate+(*(samplingCoordinates))[i].yCoordinate)]	+= 1;
			histogram[2][currentImage.GetBlue(xCoordinate+(*(samplingCoordinates))[i].xCoordinate,yCoordinate+(*(samplingCoordinates))[i].yCoordinate)]	+= 1;
		}
	}
	/*Normalize sum to 1 (maximum, next to border sum of histogram will be less than 0*/
	for (int j = 0;j<3;++j){
		for (int i = 0;i<256;++i){
			histogram[j][i] /= ((double) samplingCoordinates->size());
		}
	}
	return histogram;
}

/**Get the color of the current marker*/
unsigned char* ImagePanel::getColor(int xCoordinate,int yCoordinate){
	/*Get the histogram*/
	unsigned char* colour;
	colour = new unsigned char[3];	/*Color figure comprises 3 different colors...*/
	/*get the color values*/
	colour[0] = currentImage.GetRed(xCoordinate,yCoordinate);
	colour[1] = currentImage.GetGreen(xCoordinate,yCoordinate);
	colour[2] = currentImage.GetBlue(xCoordinate,yCoordinate);
	return colour;
}	

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
 
void ImagePanel::paintEvent(wxPaintEvent & evt)
{
    // depending on your system you may need to look at double-buffered dcs
    wxPaintDC dc(this);
    render(dc);
}
 
/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 */
void ImagePanel::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this);
    render(dc);
}
 
/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void ImagePanel::render(wxDC&  dc)
{
	//wxBufferedDC bdc = wxBufferedDC(&dc,tempImage);
    dc.DrawBitmap( resizedImage, 0, 0, false );
	//bdc.DrawBitmap( resizedImage, 0, 0, false );
}


double ImagePanel::getScalingFactor()
{
	return scaleFactor;
}

void ImagePanel::setScalingFactor(double scaleFactor)
{
	this->scaleFactor = scaleFactor;
}

/*EVENT TABLE*/
BEGIN_EVENT_TABLE(ImagePanel, wxPanel)
	EVT_PAINT(ImagePanel::paintEvent) // catch paint events
END_EVENT_TABLE()
