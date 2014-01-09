/*
	Compile under msys (or linux):
	make
	
	The following command should work as well:
	g++ Digitizer.cpp -o digitizer -static `wx-config --cppflags --libs` `pkg-config --cflags --libs libavformat` `pkg-config --cflags --libs libavcodec` `pkg-config --cflags --libs libswscale`
*/

#include <wx/wx.h>
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
	#ifdef __linux__
		XInitThreads();
	#endif
    virtual bool OnInit();
};

IMPLEMENT_APP(DigitizerApp)

bool DigitizerApp::OnInit()
{
    DigitizerFrame *frame = new DigitizerFrame( _("Digitizer"), wxPoint(0, 50), wxSize(1000, 600) );
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}

