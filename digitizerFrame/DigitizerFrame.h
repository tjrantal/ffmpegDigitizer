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

#ifndef DIGITIZERFRAME_H
#define DIGITIZERFRAME_H

#include <wx/wx.h>
#include <wx/event.h>		//Listening to mouse events
#include <wx/textctrl.h>	//Text field for displaying results
#include <wx/button.h>		//For resetting the clock
#include <wx/filedlg.h>		//For selecting a file to read
#include <wx/file.h>		//For reading a file
#include <wx/slider.h>		//For slider
#include <wx/config.h> 		//to let wxWidgets choose a wxConfig class for your platform)
#include <wx/confbase.h>	//Base config
#include <wx/fileconf.h>	//For file config; used to contain video indexing info
#include <wx/dir.h>			//Check whether a folder exists
#include <wx/filefn.h>		//Create a folder
#include <wx/filename.h>	//For saving frame indices
#include <wx/tokenzr.h>		//For splitting a string to tokens

/*Forward declare classes that are pointed to*/
class ImagePanel;
class VideoReader;
class MarkerSelector;

class DigitizerFrame: public wxFrame
{
public:
    DigitizerFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:
	//DEBUG
	FILE *debug;
	//Variables
	wxTextCtrl		*resultsText;			//For displaying text in screen
	clock_t			*clockCounts,initClock;	//For storing processor times
	wxButton		*resetButton;			//For resetting the clock
	float			elapsedTime;				//time since first click
	bool			timeRunning;				//to indicate whether the first click has passed
	FILE			*resultFile;			//File to save to
	wxButton 		*openMarkerFile;		//Pick a file to save to
	wxFile 			*openFile;
	wxButton 		*openVideoFile;		//Pick a file to save to
	wxFile 			*openVideo;
	wxSlider		*slider;				//Slider
	ImagePanel		*imagePanel;			//ImagePanel
	VideoReader		*videoReader;			//videoReader
	MarkerSelector	*markerSelector;		//Marker selector
    void OnQuit(wxCommandEvent &event);
	void OnQuit(wxCloseEvent& event);
    void OnAbout(wxCommandEvent &event);
	void LeftButtonDown(wxMouseEvent &event);
	void LeftButtonUp(wxMouseEvent &event);
	void ResetClock(wxCommandEvent &event);
	void OpenFile(wxCommandEvent &event);
	void OpenVideo(wxCommandEvent &event);
	void ScrollVideo(wxScrollEvent &event);
    DECLARE_EVENT_TABLE()
	//wxDECLARE_EVENT_TABLE();
};

enum
{
    ID_Quit = 1,
    ID_About,
	ID_Reset,
	ID_picker,
	ID_video,
	ID_slider,
	ID_panel,
	ID_markers,
};





#endif
