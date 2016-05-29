/*
	Compile under msys (or linux):
	make
	
	The following command should work as well:
	g++ Digitizer.cpp -o digitizer -static `wx-config --cppflags --libs` `pkg-config --cflags --libs libavformat` `pkg-config --cflags --libs libavcodec` `pkg-config --cflags --libs libswscale`
*/

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#include <wx/display.h>		/*Try to modify the size of the display*/
#include <wx/gdicmn.h>		/*wxRect wxDisplaySize*/
#ifndef IMAGEPANEL_H 
	#include "imagePanel/ImagePanel.h"
#endif
#ifndef DIGITIZERFRAME_H
	#include "digitizerFrame/DigitizerFrame.h"
#endif
#ifndef VIDEOREADER_H
	#include "videoReader/VideoReader.h"
#endif
#ifdef __linux__
	#include <thread>
	#include <X11/Xlib.h>
#endif

class DigitizerApp: public wxApp
{
    virtual bool OnInit();
};

//IMPLEMENT_APP(DigitizerApp)
IMPLEMENT_APP_NO_MAIN(DigitizerApp)

bool DigitizerApp::OnInit()
{
	/*Try to learn about the display on init*/
	//unsigned int displayCount = wxDisplay::GetCount();
	int width,height;
	wxDisplaySize(&width,&height);
	
    DigitizerFrame *frame = new DigitizerFrame( _("Digitizer"), wxPoint(10, 10), wxSize(width-10, height-10) );
    /*
	int currentDisplay = wxDisplay::GetFromWindow(frame);
	wxDisplay disp = wxDisplay(currentDisplay);
	wxRect rect = disp.GetGeometry();
	int width = rect.GetWidth()-50;
	int height = rect.GetHeight()-100;
	frame->SetSize(wxSize(width, height));
	*/
	frame->Show(true);
    SetTopWindow(frame);
    return true;
}

int main(int argc, char *argv[]){
	#ifdef __linux__
		XInitThreads();
	#endif
	wxEntryStart(argc,argv);
	wxTheApp->CallOnInit();
	wxTheApp->OnRun();
	return 0;
}

