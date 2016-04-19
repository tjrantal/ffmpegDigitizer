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
	#include <wx/dcbuffer.h>	/*Include for double buffering*/
	#include <math.h>       /* ceil floor round*/
	#include <vector>
	#include <mutex>		/*To use mutex to lock the thread (kept crashing when called from another thread, hope this helps...)*/
	#include "../markerSelector/MarkerCoordinates.h"
	

	class ImagePanel : public wxPanel{
		private:
			wxBitmap	imageOrig;
			wxImage		imageCopy;
			wxBitmap	resizedImage;	/**The current image displayed*/
			wxBitmap	tempImage;		/**Used for double buffering*/
			wxImage currentImage;		/**Copy of the current image for drawing onto*/
			wxSize		size;			/**On-screen image size*/
			std::mutex	lockThread;		/**Try locking the thread...*/
		public:
			wxImage currentClearImage;				/**To access the current image without overlaid color*/
			unsigned char* currentImageData = NULL;	/**Original image from videoReader*/
			unsigned char* previousImageData = NULL;	/**Previous Original image from videoReader*/
			wxSize imSize;							/**Size of the original image*/
			wxSize oldSize;							/**Size for double buffering*/
			double		scaleFactor;				/**Scaling from original to screen, use to adjust digitized coordinates*/
			/*Constructor with an image loaded*/
		    ImagePanel(wxFrame* parent,wxWindowID id, wxString file, wxBitmapType format, const wxPoint& pos, const wxSize& sizeIn);
			/*Constructor without an image*/
			ImagePanel(wxFrame* parent,wxWindowID id, const wxPoint& pos, const wxSize& sizeIn);
		    void setImage(int width, int height, unsigned char* data,bool static_data=true,bool refresh = true);	/**Set current image*/
			void digitizeXY(int xCoordinate,int yCoordinate, double radius);	/**Draw a circle in the current image*/
			void digitizeXYArea(std::vector<coordinate> areaCoordinates);		/**Highlight grown region in the current image*/
		    void paintEvent(wxPaintEvent & evt);
		    void paintNow();
			void reFreshImage();
			double getScalingFactor();								/**< Return the scaling from original image to screen*/
			void setScalingFactor(double scaleFactor);	/**< Set the screen scaling factor*/
			double** getHistogram(int xCoordinate,int yCoordinate, std::vector<coordinate> *samplingCoordinates);	/**Get the histogram of the current marker*/
			unsigned char* getColor(int xCoordinate,int yCoordinate);	/**Get the color of the current marker*/
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
