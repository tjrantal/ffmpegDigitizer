#ifndef IMAGEPANEL_H
#define IMAGEPANEL_H
	#include <wx/wx.h> /*Include all wx headers*/
	#include <wx/sizer.h>
	#include <wx/image.h>

	class ImagePanel : public wxPanel{
			wxBitmap image;
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
	BEGIN_EVENT_TABLE(ImagePanel, wxPanel)
		// some useful events
		/*
		 EVT_MOTION(wxImagePanel::mouseMoved)
		 EVT_LEFT_DOWN(wxImagePanel::mouseDown)
		 EVT_LEFT_UP(wxImagePanel::mouseReleased)
		 EVT_RIGHT_DOWN(wxImagePanel::rightClick)
		 EVT_LEAVE_WINDOW(wxImagePanel::mouseLeftWindow)
		 EVT_KEY_DOWN(wxImagePanel::keyPressed)
		 EVT_KEY_UP(wxImagePanel::keyReleased)
		 EVT_MOUSEWHEEL(wxImagePanel::mouseWheelMoved)
		 */
		EVT_PAINT(ImagePanel::paintEvent) // catch paint events
	END_EVENT_TABLE()
#endif
