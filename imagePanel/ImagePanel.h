#ifndef IMAGEPANEL_H
#define IMAGEPANEL_H
	#include <wx/wx.h> /*Include all wx headers*/
	#include <wx/sizer.h>
	#include <wx/image.h>

	class ImagePanel : public wxPanel{
			wxBitmap	imageOrig;
			wxImage		imageCopy;
			wxBitmap	resizedImage;
			double		scaleFactor;
			public:
		    ImagePanel(wxFrame* parent,wxWindowID id, wxString file, wxBitmapType format, const wxPoint& pos, const wxSize& size);
		    
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
