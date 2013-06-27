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

	class ImagePanel : public wxPanel{
			wxBitmap	imageOrig;
			wxImage		imageCopy;
			wxBitmap	resizedImage;
			wxSize		size;
			double		scaleFactor;
			public:
			/*Constructor with an image loaded*/
		    ImagePanel(wxFrame* parent,wxWindowID id, wxString file, wxBitmapType format, const wxPoint& pos, const wxSize& sizeIn);
			/*Constructor without an image*/
			ImagePanel(wxFrame* parent,wxWindowID id, const wxPoint& pos, const wxSize& sizeIn);
		    void setImage(int width, int height, unsigned char* data,bool static_data=true);
		    void paintEvent(wxPaintEvent & evt);
		    void paintNow();
		    
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
