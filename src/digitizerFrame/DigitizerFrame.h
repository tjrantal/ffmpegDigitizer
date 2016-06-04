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
#include <wx/tglbtn.h>		//Toggle button for starting and stopping auto tracking
#include <thread>			//For threading, tracking opens up a new thread
#include <vector>			//For region growing
#include <wx/grid.h>		//For results grid

/*Forward declare classes that are pointed to*/
class ImagePanel;
class VideoReader;
class MarkerSelector;
class TrackingThread;
struct coordinate;

class DigitizerFrame: public wxFrame
{
public:
	/**These variables need to be public for tracking*/
	ImagePanel		*imagePanel;			//ImagePanel
	VideoReader		*videoReader;			//videoReader
	MarkerSelector	*markerSelector;		//Marker selector
	
	wxSlider		*slider;				//Slider
	wxSlider		*searchRadius;				//Search radius from frame to next
	wxSlider		*markerRadius;				//Radius of the marker
	wxSlider		*colorTolerance;			//Tolerance for marker region grow color
	wxToggleButton	*toggleTrack;				//Begin/stop tracking
	wxToggleButton	*toggleMode;				//Digitizing mode one/all markers
	int currentFrame;						//index of currentFrame
	int previousFrame;						//index of previousFrame
	bool			allOn;			//Used to store tracking mode
	bool			trackOn;
	/**Constructor
		@param title the name for the window
		@param pos position of the window (0,0) = left upper corner at the left upper corner
		@param size size of the window
	*/
    DigitizerFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	void printCoordinates();	/**<Write the coordinates to resultsText*/
	void redrawFrame();							/**< Refresh the frame*/
private:
	int elementVerticalPos;
	//DEBUG
	FILE *debug;
	//Variables
	//wxTextCtrl		*resultsText;		/**<For coordinates*/
	wxGrid			*resultsGrid;		/**<For coordinates*/
	clock_t			*clockCounts,initClock;	/**<For storing processor times*/
	wxButton		*resetButton;			/**<For resetting the clock*/
	float			elapsedTime;				/**<time since first click*/
	bool			timeRunning;				/**<to indicate whether the first click has passed*/
	FILE			*resultFile;			/**<File to manipulate*/
	wxButton 		*openMarkerFile;		/**<Pick a file to read markers from*/
	wxFile 			*openFile;				/**<File to manipulate*/
	wxButton 		*openVideoFile;		/**<Button for picking the video to open*/
	wxFile 			*openVideo;
	wxButton 		*openSaveFile;		/**<Button for picking the coordinate save file*/
	wxTextFile		*openSave;
	wxButton		*clearMarker;		/**<Button to clear all marker coordinates*/
	wxButton		*clearOnwards;		/**<Button to clear marker coordinates from current frame onwards*/
	TrackingThread	*trackingThread;		/**<class for tracking, runs in a thread*/
	
	
	
    void OnQuit(wxCommandEvent &event);			/**< */
	void OnQuit(wxCloseEvent& event);			/**< */
    void OnAbout(wxCommandEvent &event);		/**< About*/
	void LeftButtonDown(wxMouseEvent &event);	/**< Left button handling*/
	void LeftButtonUp(wxMouseEvent &event);		/**< */
	void RightButtonDown(wxMouseEvent& event);	/**< Right button handling*/
	void KeyDown(wxKeyEvent& event);			/**< Key handling*/
	void ResetClock(wxCommandEvent &event);		/**< */
	void OpenFile(wxCommandEvent &event);		/**< Open a marker file to read markers from*/
	void OpenVideo(wxCommandEvent &event);		/**< Open a video for reading*/
	void OpenSave(wxCommandEvent &event);		/**< Open a file for saving*/
	void ScrollVideo(wxScrollEvent &event);		/**< */
	void AdjustSearchRadius(wxCommandEvent &event);	/**< */
	void AdjustMarkerRadius(wxCommandEvent &event);	/**< */
	void AdjustColorTolerance(wxCommandEvent &event);	/**< */
	void SelectMarker(wxCommandEvent &event);			/**< */
	void ToggleMode(wxCommandEvent &event);			/**< */
	void ToggleTracking(wxCommandEvent &event);			/**< */
	void ClearMarker(wxCommandEvent &event);			/**< Clear all coordinates*/
	void ClearOnwards(wxCommandEvent &event);			/**< Clear coordinates from current frame onwards*/
	void NextMarker();									/**< Proceed to Next marker*/
	void PreviousMarker();									/**< Back to Previous marker*/
	void GetMarkerValues(int marker);								/**< Get parameters of a given marker*/
    int getPrintIndices(wxFileName videoFileName,wxString videoFilePath,wxConfig *config);
	
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
	ID_save,
	ID_slider,
	ID_searchRadius,
	ID_markerRadius,
	ID_colorTolerance,
	ID_panel,
	ID_markers,
	ID_toggleMode,
	ID_toggleTracking,
	ID_clearMarker,
	ID_clearOnwards,
};





#endif
