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
	#include "DigitizerFrame.h"
#endif

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
	if (videoReader != NULL){
		delete videoReader;
	}
    Close(true);
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
		openFile = new wxFile( openFileDialog.GetPath(), wxFile::write_append );
		if (!openFile->IsOpened())
		{
			SetStatusText(_("Could not open save file!"));
			resultsText->ChangeValue(_("Could not open save file!"));
			openFile = NULL;
		}else{
			//READ MARKERS HERE
			/*Close the save file*/
			if (openFile != NULL){
				openFile->Close();
			}
		SetStatusText( _("Markers read"));
		}
	}

}

void DigitizerFrame::OpenVideo(wxCommandEvent& event){
	
	/*Open marker file*/
		wxFileDialog openFileDialog(this, _("Open video file"), _(""), _(""),
	_("Video files (*.mp4;*.avi;*.mkv)|*.mp4;*.avi;*.mkv"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL){
		SetStatusText(_("No video file opened"));
		resultsText->ChangeValue(_("No video file opened"));
	}else{
		/*Close any pre-existing video*/
		if (videoReader != NULL){
			delete videoReader;
		}
		videoReader = new VideoReader(openFileDialog.GetPath(),10);
		if (videoReader != NULL){
			printf("Frames in video %d\n",videoReader->getNumberOfFrames());
			int framesInVid = videoReader->readFrames(); 
			imagePanel->setImage(videoReader->width,videoReader->height,videoReader->video[4],true);
				SetStatusText(wxString::Format(wxT("%s %d"),"Video opened, frames", framesInVid));
		}else{
			SetStatusText(_("Could not open video!"));
			resultsText->ChangeValue(_("Could not open video!"));
		}
	}
	
}


void DigitizerFrame::ScrollVideo(wxScrollEvent &event){
	int currentVal = slider->GetValue();
	resultsText->ChangeValue(wxString::Format(wxT("%s %d"),"Frame #", currentVal));
	SetStatusText(wxString::Format(wxT("%s %d"),"Frame #", currentVal));
}

/*Event table*/
BEGIN_EVENT_TABLE(DigitizerFrame, wxFrame)
    EVT_MENU(ID_Quit,  		DigitizerFrame::OnQuit)
    EVT_MENU(ID_About, 		DigitizerFrame::OnAbout)
	EVT_SCROLL(DigitizerFrame::ScrollVideo)	
END_EVENT_TABLE()

