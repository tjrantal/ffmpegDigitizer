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
	//delete markerSelector;	//remove marker selector
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
		//If marker list does not exist yet, create it, otherwise replace with the new list
		if (markerSelector == NULL){
			markerSelector = new MarkerSelector( openFileDialog.GetPath(), this, (wxWindowID) ID_markers,wxPoint(10,250));
		} else {
			markerSelector->setMarkerList(openFileDialog.GetPath());
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
		wxString videoFilePath = openFileDialog.GetPath();
		videoReader = new VideoReader(videoFilePath.char_str());
		wxFileName videoFileName(videoFilePath);
		printf("video reader constructed\n");
		fflush(stdout);			//DEBUGGING
		if (videoReader != NULL && videoReader->videoOpen){
			SetStatusText(_("Wait while reading packet indices to memory. Requires decoding the whole file, will take a while ..."));
			printf("Read indices\n");
			fflush(stdout);			//DEBUGGING
			/*Check whether the video has already been indexed, if not index the video*/
			wxConfig *config = new wxConfig(_("digitizerConfig"));
			wxString indexFileName;
			int gotPackets = 0;
			if (config->Read(videoFilePath,&indexFileName)){
				//Read indices from file here
				printf("Reading from conf\n");
				fflush(stdout);			//DEBUGGING
				wxTextFile* indexFile = new wxTextFile();
				indexFile->Open(indexFileName,wxConvUTF8); /*Open the file for reading*/
				wxString temp;
				std::vector<frameIndice> tempFrameIndices= std::vector<frameIndice>();
				/*Read indices from the file*/
				if (indexFile->IsOpened()){
					for (temp = indexFile->GetFirstLine(); !indexFile ->Eof(); temp = indexFile->GetNextLine()){
						wxStringTokenizer tkz(temp, wxT("\t"));
						std::vector<wxString>* tokens = new std::vector<wxString>();
						while ( tkz.HasMoreTokens() )
						{
							tokens->push_back(tkz.GetNextToken());
						}
						int temp1 = wxAtoi(tokens->at(0));
						long temp2;
						long temp3;
						tokens->at(1).ToLong(&temp2);
						tokens->at(2).ToLong(&temp3);
						frameIndice tempIndice = { temp1,temp2 ,temp3};
						tempFrameIndices.push_back(tempIndice);
						tokens->clear();
						delete tokens;									
					}
	
				}
				indexFile->Close();
				delete indexFile;
				delete config;
				videoReader->frameIndices = tempFrameIndices;
			}else{
				/*Index the file and save the index file name to config*/
				gotPackets = videoReader->readIndices();
				printf("Indices read\n");
				fflush(stdout);			//DEBUGGING
				if (!wxDir::Exists(_("videoIndices"))){
					wxMkDir("videoIndices");//,0777);
					printf("Dir created\n");
					fflush(stdout);			//DEBUGGING
				}
				printf("Create file name\n");
					fflush(stdout);			//DEBUGGING
				wxFileName temp(wxT("videoIndices"));
				wxString indexFileName = temp.GetPathWithSep();
				indexFileName.Append(videoFileName.GetName());
				indexFileName.append(wxT(".ind"));
				printf("%s\n",indexFileName.ToAscii());
					fflush(stdout);			//DEBUGGING
				wxFile* indiceFile = new wxFile(indexFileName.ToAscii(),wxFile::write);
				printf("File isOpened %b\n",indiceFile->IsOpened());
					fflush(stdout);
							//DEBUGGING
				//Write indices to file here
				for (int i = 0; i<videoReader->frameIndices.size();++i){
					indiceFile->Write(wxString::Format(wxT("%d\t%ld\t%ld\n"),videoReader->frameIndices.at(i).frameNo
														,videoReader->frameIndices.at(i).pts
														,videoReader->frameIndices.at(i).pkt_pts)
														,wxConvUTF8);
				}
				indiceFile->Close();
				delete indiceFile;
				//videoReader->writeIndicesToFile();					
				config->Write(videoFilePath, indexFileName);
				delete config;
			}		
			printf("Got indices %d\n", gotPackets);
			fflush(stdout);			//DEBUGGING
			printf("Reading frame\n");
			fflush(stdout);			//DEBUGGING
			int displayPictureNumber = videoReader->readNextFrameFromDisk();			
			int framesInVid = videoReader->getNumberOfFrames();			
			printf("Frames in video %d\n",framesInVid);
			fflush(stdout);			//DEBUGGING
			
			//int gotPackets = videoReader->readPackets();
			
			//int framesInVid = videoReader->readFrames(); 
			//int displayPictureNumber = videoReader->decodeNextFrame();
			imagePanel->setImage(videoReader->width,videoReader->height,videoReader->decodedFrame,true);
				SetStatusText(wxString::Format(wxT("%s ffmpeg %d packets %d, frameNo %d"),_("Video opened, frames:"), framesInVid, videoReader->getNumberOfIndices(),displayPictureNumber));
			delete slider;
			slider = new wxSlider(this,ID_slider,0,0,videoReader->getNumberOfIndices()-1,wxPoint(300,470),wxSize(400,40));
		}else{
			SetStatusText(_("Could not open video!"));
			resultsText->ChangeValue(_("Could not open video!"));
		}
	}
	
}


void DigitizerFrame::ScrollVideo(wxScrollEvent &event){
	int currentVal = slider->GetValue();
	//int gotFrame = videoReader->decodeFrame(currentVal);
	printf("Reading frame scroller %d\n",currentVal);
	fflush(stdout);			//DEBUGGING
	//int displayPictureNumber = videoReader->readNextFrameFromDisk();
	int displayPictureNumber = videoReader->readFrameFromDisk(currentVal);
	//int displayPictureNumber = videoReader->decodeNextFrame();
	//int displayPictureNumber = videoReader->decodeFrame(currentVal);
	imagePanel->setImage(videoReader->width,videoReader->height,videoReader->decodedFrame,true);
	resultsText->ChangeValue(wxString::Format(wxT("%s %d %d"),_("Frame #"), currentVal,displayPictureNumber));
	SetStatusText(wxString::Format(wxT("%s %d %d"),_("Frame #"), currentVal,displayPictureNumber));
}

/*Event table*/
BEGIN_EVENT_TABLE(DigitizerFrame, wxFrame)
	EVT_CLOSE(DigitizerFrame::OnQuit)
    EVT_MENU(ID_Quit,  		DigitizerFrame::OnQuit)
    EVT_MENU(ID_About, 		DigitizerFrame::OnAbout)
	EVT_SCROLL(DigitizerFrame::ScrollVideo)	
END_EVENT_TABLE()

