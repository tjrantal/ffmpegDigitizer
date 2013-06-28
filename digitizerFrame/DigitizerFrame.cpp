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

#include "DigitizerFrame.h"
#include "../imagePanel/ImagePanel.h"
#include "../videoReader/VideoReader.h"
#include "../markerSelector/MarkerSelector.h"


DigitizerFrame::DigitizerFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame(NULL, -1, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;

    menuFile->Append( ID_About, _("&About...") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_Quit, _("E&xit") );

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, _("&File") );

    SetMenuBar( menuBar );

    CreateStatusBar();
    SetStatusText( _("Welcome to Digitizer!") );
	/*Add text field for results*/
	resultsText = new wxTextCtrl(this,-1,_("Button hasn't been pressed"),wxPoint(10,50),wxSize(200,40),wxTE_MULTILINE);
	/*Button for reading markers in*/
	openMarkerFile 	= new wxButton(this,ID_picker,_("Open marker file"),wxPoint(10,150));
	Connect(ID_picker,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(DigitizerFrame::OpenFile),NULL,this);
	openVideoFile 	= new wxButton(this,ID_video,_("Open video file"),wxPoint(10,200));
	Connect(ID_video,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(DigitizerFrame::OpenVideo),NULL,this);
	/*Slider for browsing video*/
	slider = new wxSlider(this,ID_slider,0,0,100,wxPoint(300,470),wxSize(400,40));
	//Connect(ID_slider,EVT_SCROLL,wxCommandEventHandler(DigitizerFrame::ScrollVideo),NULL,this);
	/*Connect button click events*/
	resultsText->Connect(wxEVT_LEFT_DOWN,wxMouseEventHandler(DigitizerFrame::LeftButtonDown), NULL,this);
	resultsText->Connect(wxEVT_LEFT_UP,wxMouseEventHandler(DigitizerFrame::LeftButtonUp),NULL,this);
	/*Connect events to the main frame*/
	Connect(wxEVT_LEFT_DOWN,wxMouseEventHandler(DigitizerFrame::LeftButtonDown), NULL,this);
	Connect(wxEVT_LEFT_UP,wxMouseEventHandler(DigitizerFrame::LeftButtonUp),NULL,this);
	
	//imagePanel = new ImagePanel(this,ID_panel,_("DSC_0001.JPG"),wxBITMAP_TYPE_JPEG,wxPoint(200,10),wxSize(750,380));
	imagePanel = new ImagePanel(this,ID_panel,wxPoint(200,10),wxSize(750,380));
	//videoReader = new VideoReader("GOPR0085.MP4",10);
	videoReader = NULL;
	markerSelector = NULL;
	//debug = freopen("debug.log","w",stdout);
}

/*Button event handling*/
void DigitizerFrame::LeftButtonDown(wxMouseEvent& WXUNUSED(event)){
	//does nothing
}

void DigitizerFrame::LeftButtonUp(wxMouseEvent& WXUNUSED(event)){
	//Does nothing
}

void DigitizerFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	SetStatusText( _("Tried_about") );
}

void DigitizerFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void DigitizerFrame::OnQuit(wxCloseEvent &event)
{
	printf("Delete vReader at the end\n");
	fflush(stdout);	//DEBUGGING	
	if (videoReader != NULL){
		delete videoReader;
		videoReader = NULL;
	}
	printf("Nearly done\n");
	fflush(stdout);	//DEBUGGING
	//fclose(debug);	//DEBUGGING
 	wxWindow::Destroy();
	//event.Skip();
}

void DigitizerFrame::OpenFile(wxCommandEvent& event){

	/*Open marker file*/
	wxFileDialog openFileDialog(this, _("Open TAB file"), _(""), _(""),
	_("TAB files (*.tab)|*.tab"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL){
		SetStatusText(_("No marker file opened"));
		resultsText->ChangeValue(_("No marker file opened"));
	}else{
		// save the current contents in the file;
		// this can be done with e.g. wxWidgets output streams:
		if (markerSelector == NULL){
			markerSelector = new MarkerSelector( openFileDialog.GetPath(), this, (wxWindowID) ID_markers,wxPoint(10,250));
		}
		if (markerSelector == NULL)
		{
			SetStatusText(_("Could not open markers!"));
			resultsText->ChangeValue(_("Could not open markers!"));
			openFile = NULL;
		}else{
			SetStatusText( _("Markers read"));
		}
	}

}

void DigitizerFrame::OpenVideo(wxCommandEvent& event){
	
	/*Open marker file*/
		wxFileDialog openFileDialog(this, _("Open video file"), _(""), _(""),
	_("Video files (*.mp4;*.avi;*.mkv)|*.mp4;*.MP4;*.avi;*.AVI;*.mkv;*.MKV"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL){
		SetStatusText(_("No video file opened"));
		resultsText->ChangeValue(_("No video file opened"));
	}else{
		/*Close any pre-existing video*/
		printf("Pre-existing videoReader?\n");
		fflush(stdout);			//DEBUGGING
		if (videoReader != NULL){
			printf("Remove pre-existing videoReader?\n");
			fflush(stdout);			//DEBUGGING
			delete videoReader;
					printf("Return from deconstructor\n");
		fflush(stdout);			//DEBUGGING
			videoReader = NULL;
		}
		printf("construct video reader\n");
		fflush(stdout);			//DEBUGGING
		videoReader = new VideoReader(openFileDialog.GetPath().char_str(),10);
		printf("video reader constructed\n");
		fflush(stdout);			//DEBUGGING
		if (videoReader != NULL && videoReader->videoOpen){
			printf("Frames in video %d\n",videoReader->getNumberOfFrames());
			int framesInVid = videoReader->readFrames(); 
			imagePanel->setImage(videoReader->width,videoReader->height,videoReader->video[4],true);
				SetStatusText(wxString::Format(wxT("%s %d"),_("Video opened, frames"), framesInVid));
		}else{
			SetStatusText(_("Could not open video!"));
			resultsText->ChangeValue(_("Could not open video!"));
		}
	}
	
}


void DigitizerFrame::ScrollVideo(wxScrollEvent &event){
	int currentVal = slider->GetValue();
	resultsText->ChangeValue(wxString::Format(wxT("%s %d"),_("Frame #"), currentVal));
	SetStatusText(wxString::Format(wxT("%s %d"),_("Frame #"), currentVal));
}

/*Event table*/
BEGIN_EVENT_TABLE(DigitizerFrame, wxFrame)
	EVT_CLOSE(DigitizerFrame::OnQuit)
    EVT_MENU(ID_Quit,  		DigitizerFrame::OnQuit)
    EVT_MENU(ID_About, 		DigitizerFrame::OnAbout)
	EVT_SCROLL(DigitizerFrame::ScrollVideo)	
END_EVENT_TABLE()

