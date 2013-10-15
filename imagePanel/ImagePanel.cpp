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
    	resizedImage = *(new wxBitmap(imageCopy));
    }else{ //No scaling needed
    	resizedImage = *(new wxBitmap(imageOrig));
    }
    
}

ImagePanel::ImagePanel(wxFrame* parent,wxWindowID id, const wxPoint& pos, const wxSize& sizeIn) :
wxPanel(parent,id,pos,sizeIn)
{
    size = sizeIn;
}


/*Set image to show*/
void ImagePanel::setImage(int width, int height, unsigned char* data,bool static_data){
	imageCopy =*(new wxImage(width,height,data,static_data));
	scaleFactor = (double)width/(double)size.GetWidth() > (double)height/(double)size.GetHeight() ? (double)width/(double)size.GetWidth() : (double)height/(double)size.GetHeight();
    if (scaleFactor != 1){ //Scale to fit the panel
    	imageCopy.Rescale(width/scaleFactor,height/scaleFactor,wxIMAGE_QUALITY_HIGH);
    	resizedImage = *(new wxBitmap(imageCopy));
    }else{ //No scaling needed
    	resizedImage = *(new wxBitmap(imageOrig));
    }
    Refresh();

}
 
 /**Draw a circle in the current image to highlight a digitized marker*/
 void ImagePanel::digitizeXY(int xCoordinate,int yCoordinate, double radius){
	wxImage tempImage = resizedImage.ConvertToImage();
	/*draw to the image*/
	double xAdd;
	double yAdd;
	for  (int i = 0; i<(int)ceil(2.0*M_PI*radius);++i){
		xAdd = radius*cos(((double)i)/(radius));
		yAdd = radius*sin(((double)i)/(radius));
		tempImage.SetRGB(xCoordinate+(int)xAdd,yCoordinate+(int)yAdd,(unsigned char) 255,(unsigned char) 0,(unsigned char) 0);
	}
	resizedImage = *(new wxBitmap(tempImage));
	Refresh();
} 

/**Get the histogram of the current marker*/
double** ImagePanel::getHistogram(int xCoordinate,int yCoordinate, double radius){
	/*Calculate the circular sampling*/
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
	/*Get the histogram*/
	double** histogram;
	histogram = new double*[3];	/*Color figure comprises 3 different colors...*/
	for (int i = 0; i<3;++i){
		histogram[i] = new double[256](); /*256 possible intensities of a given color, the () initialises the values to zero*/
	}
	/*get the colorvalues for the histograms*/
	wxImage tempImage = resizedImage.ConvertToImage();
	for (int i = 0; i<samplingCoordinates.size(); ++i){
		if (xCoordinate+samplingCoordinates[i][0] >=0 && xCoordinate+samplingCoordinates[i][0] < size.GetWidth()
			&& xCoordinate+samplingCoordinates[i][1] >=0 && xCoordinate+samplingCoordinates[i][1] < size.GetHeight()
			){
			histogram[0][tempImage.GetRed(xCoordinate+samplingCoordinates[i][0],yCoordinate+samplingCoordinates[i][1])]		+= 1;
			histogram[1][tempImage.GetGreen(xCoordinate+samplingCoordinates[i][0],yCoordinate+samplingCoordinates[i][1])]	+= 1;
			histogram[2][tempImage.GetBlue(xCoordinate+samplingCoordinates[i][0],yCoordinate+samplingCoordinates[i][1])]	+= 1;
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
    dc.DrawBitmap( resizedImage, 0, 0, false );
}


double ImagePanel::getScalingFactor()
{
	return scaleFactor;
}

/*EVENT TABLE*/
BEGIN_EVENT_TABLE(ImagePanel, wxPanel)
	// some useful events
	/*
	 EVT_MOTION(ImagePanel::mouseMoved)
	 EVT_LEFT_DOWN(ImagePanel::mouseDown)
	 EVT_LEFT_UP(ImagePanel::mouseReleased)
	 EVT_RIGHT_DOWN(ImagePanel::rightClick)
	 EVT_LEAVE_WINDOW(ImagePanel::mouseLeftWindow)
	 EVT_KEY_DOWN(ImagePanel::keyPressed)
	 EVT_KEY_UP(ImagePanel::keyReleased)
	 EVT_MOUSEWHEEL(ImagePanel::mouseWheelMoved)
	 */
	EVT_PAINT(ImagePanel::paintEvent) // catch paint events
END_EVENT_TABLE()
