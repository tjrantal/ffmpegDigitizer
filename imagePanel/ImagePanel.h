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
#define IMAGEPANEL_H
	#include <wx/wx.h> /*Include all wx headers*/
	#include <wx/sizer.h>
	#include <wx/image.h>
	#include <math.h>       /* ceil floor round*/
	#include <vector>
	

	class ImagePanel : public wxPanel{
		private:
			wxBitmap	imageOrig;
			wxImage		imageCopy;
			wxBitmap	resizedImage;	/**The current image displayed*/
			wxImage currentImage;		/**Copy of the current image for drawing onto*/
			wxSize		size;			/**On-screen image size*/
			double		scaleFactor;	/**Scaling from original to screen*/
		public:
			wxImage currentClearImage;	/**To access the current image without overlaid color*/
			/*Constructor with an image loaded*/
		    ImagePanel(wxFrame* parent,wxWindowID id, wxString file, wxBitmapType format, const wxPoint& pos, const wxSize& sizeIn);
			/*Constructor without an image*/
			ImagePanel(wxFrame* parent,wxWindowID id, const wxPoint& pos, const wxSize& sizeIn);
		    void setImage(int width, int height, unsigned char* data,bool static_data=true);	/**Set current image*/
			void digitizeXY(int xCoordinate,int yCoordinate, double radius);	/**Draw a circle in the current image*/
		    void paintEvent(wxPaintEvent & evt);
		    void paintNow();
			double getScalingFactor(); /**Return the scaling from original image to screen*/
			double** getHistogram(int xCoordinate,int yCoordinate, std::vector<int*> samplingCoordinates);	/**Get the histogram of the current marker*/
		    
		    void render(wxDC& dc);
		    
		    // some useful events
		    /*
		     void mouseMoved(wxMouseEvent& event);
		     void mouseDown(wxMouseEvent& event);
		     void mouseWheelMoved(wxMouseEvent& event);
		     void mouseReleased(wxMouseEvent& event);
		     void rightClick(wxMouseEvent& event);
		     void mouseLeftWindow(wxMouseEvent& event);
		     void keyPressed(wxKeyEvent& event);
		     void keyReleased(wxKeyEvent& event);
		     */
		    DECLARE_EVENT_TABLE()
		    
	};

#endif
