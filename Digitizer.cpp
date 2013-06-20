/*
	Compile under msys (or linux):
	make
	
	The following command should work as well:
	g++ Digitizer.cpp -o digitizer -static `wx-config --cppflags --libs` `pkg-config --cflags --libs libavformat` `pkg-config --cflags --libs libavcodec` `pkg-config --cflags --libs libswscale`
*/

#include <wx/wx.h>
#include "guiImplementation/DigitizerFrame.h"

class DigitizerApp: public wxApp
{
    virtual bool OnInit();
};



BEGIN_EVENT_TABLE(DigitizerFrame, wxFrame)
    EVT_MENU(ID_Quit,  		DigitizerFrame::OnQuit)
    EVT_MENU(ID_About, 		DigitizerFrame::OnAbout)
	EVT_SCROLL(DigitizerFrame::ScrollVideo)	
END_EVENT_TABLE()

IMPLEMENT_APP(DigitizerApp)

bool DigitizerApp::OnInit()
{
    DigitizerFrame *frame = new DigitizerFrame( _("Digitizer"), wxPoint(0, 50), wxSize(1000, 600) );
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}

