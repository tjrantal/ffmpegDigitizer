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

/*Include dependent classess and structs*/
#include "DigitizerFrame.h"
#include "../imagePanel/ImagePanel.h"
#include "../videoReader/VideoReader.h"
#include "../markerSelector/MarkerCoordinates.h"
#include "../markerSelector/MarkerSelector.h"
#include "../trackingThread/TrackingThread.h"
#include <vector>		
#include <string>      
#include <sstream>
#include <math.h>       /* isnan, sqrt */

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
	//resultsText = new wxTextCtrl(this,-1,_("Coordinates will appear here"),wxPoint(10,10),wxSize(180,130),wxTE_MULTILINE);
	//resultsGrid = new wxGrid(this,-1,wxPoint(10,10),wxSize(180,100),wxVSCROLL|wxHSCROLL|wxWANTS_CHARS|wxFULL_REPAINT_ON_RESIZE,_("Coordinates"));
	//resultsGrid = new wxGrid(this, -1, wxPoint(10, 10), wxSize(180, 100), (long) (wxVSCROLL | wxHSCROLL | wxWANTS_CHARS | wxFULL_REPAINT_ON_RESIZE), _("Coordinates"));

	/*
	    wxGrid(wxWindow *parent,
            wxWindowID id,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize,
				long style = wxWANTS_CHARS,
				const wxString& name = wxGridNameStr)
				*/
	//resultsGrid->CreateGrid(4,4);
	/*Button for reading markers in*/
	openMarkerFile 	= new wxButton(this,ID_picker,_("Open &marker file"),wxPoint(10,10));
	Connect(ID_picker,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(DigitizerFrame::OpenFile),NULL,this);
	openVideoFile 	= new wxButton(this,ID_video,_("Open &video file"),wxPoint(10,60));
	Connect(ID_video,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(DigitizerFrame::OpenVideo),NULL,this);
	openSaveFile 	= new wxButton(this,ID_save,_("Open &coordinate save file"),wxPoint(10,100));
	Connect(ID_save,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(DigitizerFrame::OpenSave),NULL,this);
	openSave = NULL; /*Init the result file to null*/
	/*Slider for browsing video*/
	slider = NULL;
	//slider = new wxSlider(this,ID_slider,0,0,100,wxPoint(300,470),wxSize(400,40));
	//Connect(ID_slider,EVT_SCROLL,wxCommandEventHandler(DigitizerFrame::ScrollVideo),NULL,this);
	/*Connect button click events*/
	/*
	resultsText->Connect(wxEVT_LEFT_DOWN,wxMouseEventHandler(DigitizerFrame::LeftButtonDown), NULL,this);
	resultsText->Connect(wxEVT_LEFT_UP,wxMouseEventHandler(DigitizerFrame::LeftButtonUp),NULL,this);
	*/
	/*Connect events to the main frame*/
	/*
	Connect(wxEVT_LEFT_DOWN,wxMouseEventHandler(DigitizerFrame::LeftButtonDown), NULL,this);
	Connect(wxEVT_LEFT_UP,wxMouseEventHandler(DigitizerFrame::LeftButtonUp),NULL,this);
	*/
	//imagePanel = new ImagePanel(this,ID_panel,_("DSC_0001.JPG"),wxBITMAP_TYPE_JPEG,wxPoint(200,10),wxSize(750,380));
	/*Maximize imagePanel size*/
	int width,height;
	this->GetSize(&width,&height);	
	
	imagePanel = new ImagePanel(this,ID_panel,wxPoint(200,10),wxSize(width-220,height-180));
	/*Connect the mouse listener to digitize points on screen*/
	imagePanel->Connect(wxEVT_LEFT_DOWN,wxMouseEventHandler(DigitizerFrame::LeftButtonDown), NULL,this);
	imagePanel->Connect(wxEVT_RIGHT_DOWN,wxMouseEventHandler(DigitizerFrame::RightButtonDown), NULL,this);
	
		/*Text panel for digitized markers*/
		//resultsText = new wxTextCtrl(this,-1,_("Coordinates will appear here"),wxPoint(1000,10),wxSize(200,200),wxTE_MULTILINE);
	/*Key handling*/
	Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(DigitizerFrame::KeyDown),NULL,this);
	imagePanel->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(DigitizerFrame::KeyDown),NULL,this);
	videoReader = NULL;
	markerSelector = NULL;
		//debug = freopen("debug.log","w",stdout);
}

/*Button event handling*/
/*Digitizing markers*/
void DigitizerFrame::LeftButtonDown(wxMouseEvent& event){
	/*Adjust the digitized coordinates with the sceling factor*/
	printf("Left Button %d %d\n", event.GetX(), event.GetY());
	SetStatusText(wxString::Format(wxT("X %i Y %i"),event.GetX(), event.GetY() ));
	
	double xCoordinate = ((double) event.GetX())*imagePanel->scaleFactor;
	double yCoordinate = ((double) event.GetY())*imagePanel->scaleFactor;
	double radius = (double)  markerRadius->GetValue();
	SetStatusText(wxString::Format(wxT("X %i Y %i X %f Y %f"),event.GetX(), event.GetY(),xCoordinate,yCoordinate ));
	
	//printf("Trying to digitizeXY\n");
	imagePanel->digitizeXY((int) xCoordinate,(int) yCoordinate, radius);

	//Get active marker and set the coordinates for the marker

	int selectedMarker = markerSelector->GetCurrentSelection();	//Number of active marker
	markerSelector->setCoordinate(selectedMarker,xCoordinate, yCoordinate, slider->GetValue());	//Set the coordinate for the frame
	//Take the histogram for the marker
	//markerSelector->markers[selectedMarker].histogram = imagePanel->getHistogram(xCoordinate, yCoordinate, markerSelector->markers[selectedMarker].radiusCoordinates);
	//markerSelector->markers[selectedMarker].fourBitColors = imagePanel->getColor(xCoordinate,yCoordinate);
	//printf("Try to get color\n");
	markerSelector->markers[selectedMarker].fourBitColors = TrackingThread::getColor(imagePanel->currentImageData, imagePanel->imSize.x, imagePanel->imSize.y,(int) xCoordinate,(int) yCoordinate);
	//Highlight area...
	//std::vector<coordinate> areaCoordinates = TrackingThread::growRegion(new wxImage(imagePanel->currentClearImage),xCoordinate,yCoordinate,markerSelector->markers[selectedMarker].fourBitColors,markerSelector->markers[selectedMarker].colorTolerance);
	//printf("Got color, try to grow region\n");
	std::vector<coordinate> areaCoordinates = TrackingThread::growRegion(imagePanel->currentImageData, imagePanel->imSize.x, imagePanel->imSize.y,xCoordinate,yCoordinate,markerSelector->markers[selectedMarker].fourBitColors,markerSelector->markers[selectedMarker].colorTolerance);
	//printf("Grew region, trying to digitizer area\n");
	redrawFrame();	//Erase the previous digitizations
	imagePanel->digitizeXYArea(areaCoordinates);
	imagePanel->reFreshImage();
	//printf("Digitized area\n");	
	
}

void DigitizerFrame::RightButtonDown(wxMouseEvent& event){
	NextMarker();
}

void DigitizerFrame::LeftButtonUp(wxMouseEvent& WXUNUSED(event)){
	//Does nothing
}

void DigitizerFrame::KeyDown(wxKeyEvent& event){
	//Keyboard short cuts
	int charCode = event.GetKeyCode();
	printf("Got keycode %c\n",(char) charCode);
	wxCommandEvent emptyEvent = wxCommandEvent();
	//m marker
	if (charCode == 109 ||charCode == 77){
		OpenFile(emptyEvent );
		return;
	}
	//v = video
	if (charCode == 118 ||charCode == 86){
		OpenVideo(emptyEvent);
		return;
	}
	//c = coordinate
	if (charCode == 99 ||charCode == 67){
		OpenSave(emptyEvent);
		return;
	}
	//n = next marker
	if (charCode == 110 ||charCode == 78){
		NextMarker();
		return;
	}
	//p = previous marker
	if (charCode == 112||charCode == 80){
		PreviousMarker();
		return;
	}
	
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
	printf("Deleted vReader at the end\n");
	/*Write out and close any open results file*/
	if (openSave != NULL && openSave->IsOpened()){
		printf("Close openSave\n");
		//printCoordinates();
		openSave->Write();
		openSave->Close();
		delete openSave;
		openSave = NULL;
	}
	
	printf("Nearly done\n");
	fflush(stdout);	//DEBUGGING
	//fclose(debug);	//DEBUGGING
 	wxWindow::Destroy();
	//event.Skip();
}

void DigitizerFrame::OpenSave(wxCommandEvent& event){
	//Use wxConfig to store path to look for coordinate file... 
	wxConfig *config = new wxConfig(_("digitizerConfig"));
	wxString defaultCoordinateFolder;
	if (config->Read(_("defaultCoordinateFolder"),&defaultCoordinateFolder)){
	}else{
		defaultCoordinateFolder = _("");
	}
	
	/*Open coordinate file*/
	wxFileDialog openFileDialog(this, _("Create/open a RES file. Will be overwritten"), defaultCoordinateFolder, _(""),
	_("RES files (*.RES|*.res"), wxFD_SAVE);//|wxFD_OVERWRITE_PROMPT);
	if (openFileDialog.ShowModal() == wxID_CANCEL){
		SetStatusText(_("No marker file opened"));
		delete config;
		//resultsText->ChangeValue(_("No marker file opened"));
	}else{
		//Save default defaultMarkerFolder
		wxFileName temp(openFileDialog.GetPath());
		config->Write(_("defaultCoordinateFolder"),temp.GetPath());
		delete config;
		//If marker list does not exist yet, create it, otherwise replace with the new list
		if (openSave != NULL){
			openSave->Write();
			openSave->Close();
			delete openSave;
		}
		
		try{
			openSave = new wxTextFile(openFileDialog.GetPath());
			if (openSave->Exists()){
				openSave->Open();
				//Read marker coordinates from file
				wxString headerLine = openSave->GetFirstLine();	//Ignore
				wxString nextLine;
				
				while (!openSave->Eof()){
					nextLine = openSave->GetNextLine();
					std::vector<std::string> delimited;
					std::string tempLine(nextLine);
					std::stringstream sourceStringStream(tempLine);
					std::string item;
					while(std::getline(sourceStringStream,item,'\t')){
						delimited.push_back(item);
						//cout<<item<<"\t";
					}
					for( int i = 1;i<delimited.size();i=i+2){
						int markerIndice = (i-1)/2;
						double xCoord = std::stod(delimited.at(i),nullptr);
						double yCoord = std::stod(delimited.at(i+1),nullptr);
						int frameInd = std::stoi(delimited.at(0),nullptr);
						if ((xCoord==xCoord) && (yCoord==yCoord)){
							//printf("fi %d marker %d x %.1f y %.1f\n",frameInd,markerIndice,xCoord,yCoord);
							markerSelector->setCoordinate(markerIndice,xCoord,yCoord,frameInd);
						}
						
					}
				}
				redrawFrame();	//Draw the coordinate on the frame
				
				SetStatusText(wxString::Format(wxT("opened %s for makers %i"),openFileDialog.GetPath().c_str(),openSave->IsOpened() ));
				//printf("File exists, opened %d\n",openSave->IsOpened());
			}else{
				//printf("File did not exist %d\n",openSave->IsOpened());
				openSave->Create();
				//printf("File created %d\n",openSave->IsOpened());
				openSave->Open();
				SetStatusText(wxString::Format(wxT("created %s for makers %i"),openFileDialog.GetPath().c_str(),openSave->IsOpened() ));
				//printf("Created file opened %d\n",openSave->IsOpened());
			}
		}catch (...){
			SetStatusText(_("Could not open or create a res file"));
		}
		/*
		if (openSave->IsOpened()){
			SetStatusText(wxString::Format(wxT(" opened %s for makers"),openFileDialog.GetPath().mb_str()));
		} else {
			SetStatusText(_("Could not open the res file"));
		}
		*/
	}
}

void DigitizerFrame::OpenFile(wxCommandEvent& event){
	//Use wxConfig to store path to look for maker file... 
	wxConfig *config = new wxConfig(_("digitizerConfig"));
	wxString defaultMarkerFolder;
	if (config->Read(_("defaultMarkerFolder"),&defaultMarkerFolder)){
	}else{
		defaultMarkerFolder = _("");
	}
	
	/*Open marker file*/
	wxFileDialog openFileDialog(this, _("Open TAB file"), defaultMarkerFolder, _(""),
	_("TAB files (*.tab)|*.tab"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL){
		SetStatusText(_("No marker file opened"));
		delete config;
		//resultsText->ChangeValue(_("No marker file opened"));
	}else{
		//Save default defaultMarkerFolder
		wxFileName temp(openFileDialog.GetPath());
		config->Write(_("defaultMarkerFolder"),temp.GetPath());
		delete config;
		//If marker list does not exist yet, create it, otherwise replace with the new list
		if (markerSelector == NULL){
			markerSelector = new MarkerSelector( openFileDialog.GetPath(), this, (wxWindowID) ID_markers,wxPoint(10,150));
			/*Connect event listener to the drop down menu for when the selection is cahnged*/
			Connect(wxEVT_COMMAND_COMBOBOX_SELECTED,wxCommandEventHandler(DigitizerFrame::SelectMarker), NULL,this);
			/*Add sliders for markers*/
			markerRadius = new wxSlider(this,ID_markerRadius,10,1,50,wxPoint(10,200),wxSize(100,40),wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS);
			searchRadius = new wxSlider(this,ID_searchRadius,30,1,100,wxPoint(10,250),wxSize(100,40),wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS);
			colorTolerance = new wxSlider(this,ID_colorTolerance,30,0,255,wxPoint(10,300),wxSize(100,40),wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS);
			toggleTrack = new wxToggleButton(this,ID_toggleTracking,_("&Toggle Tracking"),wxPoint(10,350));
			clearMarker		 = new wxButton(this,ID_clearMarker,_("&Clear Marker"),wxPoint(10,400));
			clearOnwards	 = new wxButton(this,ID_clearOnwards,_("&Clear Onwards"),wxPoint(10,450));
			//Connect callbacks
			Connect(ID_clearMarker,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(DigitizerFrame::ClearMarker),NULL,this);
			Connect(ID_clearOnwards,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(DigitizerFrame::ClearOnwards),NULL,this);
	
			
			trackOn = false;
		} else {
			markerSelector->setMarkerList(openFileDialog.GetPath());
			trackOn = false;
		}
		if (markerSelector == NULL)
		{
			SetStatusText(_("Could not open markers!"));
			//resultsText->ChangeValue(_("Could not open markers!"));
			openFile = NULL;
		}else{
			SetStatusText( _("Markers read"));
			
			
		}
	}

}

void DigitizerFrame::OpenVideo(wxCommandEvent& event){
	//Use wxConfig to store path to look for videos... 
	wxConfig *config = new wxConfig(_("digitizerConfig"));
	wxString defaultVideoFolder;
	if (config->Read(_("defaultVideoFolder"),&defaultVideoFolder)){
	}else{
		defaultVideoFolder = _("");
	}
	
	/*Open marker file*/
		wxFileDialog openFileDialog(this, _("Open video file"), defaultVideoFolder, _(""),
	_("Video files (*.mp4;*.avi;*.mkv)|*.mp4;*.MP4;*.avi;*.AVI;*.mkv;*.MKV"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL){
		SetStatusText(_("No video file opened"));
		delete config;
		//resultsText->ChangeValue(_("No video file opened"));
	}else{
				/*Close any pre-existing video*/
		//printf("Pre-existing videoReader?\n");
		fflush(stdout);			//DEBUGGING
		if (videoReader != NULL){
			//printf("Remove pre-existing videoReader?\n");
			fflush(stdout);			//DEBUGGING
			delete videoReader;
					printf("Return from destructor\n");
		fflush(stdout);			//DEBUGGING
			videoReader = NULL;
		}
		//printf("construct video reader\n");
		fflush(stdout);			//DEBUGGING
		wxString videoFilePath = openFileDialog.GetPath();
		videoReader = new VideoReader(videoFilePath.char_str());
		wxFileName videoFileName(videoFilePath);
		//Save the video file folder as the default path
		config->Write(_("defaultVideoFolder"), videoFileName.GetPath());
		//printf("video reader constructed\n");
		fflush(stdout);			//DEBUGGING
		if (videoReader != NULL && videoReader->videoOpen){
			SetStatusText(_("Wait while reading packet indices to memory. Requires decoding the whole file, will take a while ..."));
			//printf("Read indices\n");
			fflush(stdout);			//DEBUGGING
			/*Check whether the video has already been indexed, if not index the video*/
			//config = new wxConfig(_("digitizerConfig"));
			wxString indexFileName;
			int gotPackets = 0;
			if (config->Read(videoFilePath,&indexFileName)){
				//Read indices from file here
				printf("Reading from conf %s\n",indexFileName.c_str().AsChar());
				fflush(stdout);			//DEBUGGING
				wxTextFile* indexFile = new wxTextFile();
				if(wxFileName::FileExists(indexFileName)){
					indexFile->Open(indexFileName,wxConvUTF8); /*Open the file for reading*/
					wxString temp;
					std::vector<FrameIndice*> *tempFrameIndices = new std::vector<FrameIndice*>();
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
							long temp4;
							long temp5;
							tokens->at(1).ToLong(&temp2);
							tokens->at(2).ToLong(&temp3);
							tokens->at(3).ToLong(&temp4);
							tokens->at(4).ToLong(&temp5);
							FrameIndice *tempIndice = new FrameIndice( temp1,temp2 ,temp3,temp4,temp5);
							tempFrameIndices->push_back(tempIndice);
							tokens->clear();
							delete tokens;									
						}
		
					}
					indexFile->Close();
					delete indexFile;
					videoReader->frameIndices = *tempFrameIndices;
				} else{
					//printf("File didn't exist\n");
					gotPackets = getPrintIndices(videoFileName,videoFilePath,config);
				}
			}else{
				//printf("File not found from config\n");
				gotPackets = getPrintIndices(videoFileName,videoFilePath,config);
			}
			delete config;
			
			//int displayPictureNumber = videoReader->readNextFrameFromDisk();
			printf("Try to read the first frame\n");
			int displayPictureNumber = videoReader->readFrameFromDisk(0);
			printf("Tried to read the first frame\n");
			imagePanel->setImage(videoReader->width,videoReader->height,videoReader->decodedFrame,true);
				SetStatusText(wxString::Format(wxT("%s ffmpeg packets %d, frameNo %d"),_("Video opened, frames:"), videoReader->getNumberOfIndices(),displayPictureNumber));

			if (slider != NULL){
				delete slider;
			}
			int width,height;
			this->GetSize(&width,&height);
			slider = new wxSlider(this,ID_slider,0,0,videoReader->getNumberOfIndices()-1,wxPoint((width-200)/2-200,height-120),wxSize(400,40),wxSL_HORIZONTAL | wxSL_AUTOTICKS | wxSL_LABELS);
			currentFrame = 0;
		}else{
			SetStatusText(_("Could not open video!"));
			//resultsText->ChangeValue(_("Could not open video!"));
		}
	}
	
}

int DigitizerFrame::getPrintIndices(wxFileName videoFileName,wxString videoFilePath,wxConfig *config){
	/*Index the file and save the index file name to config*/
	int gotPackets = videoReader->readIndices();
	//printf("Indices read\n");
	fflush(stdout);			//DEBUGGING
	if (!wxDir::Exists(_("videoIndices"))){
		#ifdef __linux__
			wxMkDir("videoIndices",0777);	//Linux
		#else
			wxMkDir("videoIndices");		//Windows
		#endif
		//printf("Dir created\n");
		fflush(stdout);			//DEBUGGING
	}
	//printf("Create file name\n");
		fflush(stdout);			//DEBUGGING
	wxDir temp(wxT("videoIndices"));
	wxString indexFileName = temp.GetNameWithSep();
	indexFileName.Append(videoFileName.GetFullName());
	indexFileName.append(wxT(".ind"));
	printf("INDEX FILE NAME %s\n",indexFileName.c_str().AsChar());

	/**Couldn't figure out how to convert to appropriate string type using one command on both linux and windows. Could be a difference between different wxWidgets versions as well, didn't check...*/
	#ifdef __linux__
		wxFile* indiceFile = new wxFile(indexFileName.wc_str(),wxFile::write);	//Linux
	#else
		//wxFile* indiceFile = new wxFile(indexFileName.ToAscii(),wxFile::write);		//Windows
		wxFile* indiceFile = new wxFile(indexFileName.c_str().AsChar(), wxFile::write);		//Windows		
	#endif
	printf("File isOpened %d\n",indiceFile->IsOpened());
	//	fflush(stdout);
				//DEBUGGING
	//Write indices to file here
	for (int i = 0; i<videoReader->frameIndices.size();++i){
		indiceFile->Write(wxString::Format(wxT("%d\t%ld\t%ld\t%ld\t%ld\n"), (int) videoReader->frameIndices.at(i)->frameNo
							, (long) videoReader->frameIndices.at(i)->pts
							, (long) videoReader->frameIndices.at(i)->pkt_pts
							, (long) videoReader->frameIndices.at(i)->dts
							, (long) videoReader->frameIndices.at(i)->bestPTS));
											//,wxConvUTF8);
	}
	indiceFile->Close();
	delete indiceFile;
	//videoReader->writeIndicesToFile();
	printf("Wrote indiceFile\n");
	config->Write(videoFilePath, indexFileName.c_str().AsChar());
	printf("Saved config\n");
	return gotPackets;
}

void DigitizerFrame::ScrollVideo(wxScrollEvent &event){
	currentFrame = slider->GetValue();
	int displayPictureNumber = videoReader->readFrameFromDisk(currentFrame);
	redrawFrame();
	//resultsText->ChangeValue(wxString::Format(wxT("%s %d %d"),_("Frame #"), currentFrame,displayPictureNumber));
	SetStatusText(wxString::Format(wxT("%s %d %d"),_("Frame #"), currentFrame,displayPictureNumber));
}

void DigitizerFrame::redrawFrame(){
	imagePanel->setImage(videoReader->width,videoReader->height,videoReader->decodedFrame,true);
	/*Check whether markers exist, if so add them to the image*/
	if (markerSelector != NULL){
		for (int i = 0; i<markerSelector->markers.size();++i){
			try{
				coordinate tempCoordinate = markerSelector->getCoordinate(i,currentFrame);
				imagePanel->digitizeXY((int) tempCoordinate.xCoordinate,(int)  tempCoordinate.yCoordinate, markerSelector->markers[i].markerRadius);
			} catch (...){
			
			}
		}
	}
}
/**<Write the coordinates to resultsText and to the results file (openSave)*/

void DigitizerFrame::printCoordinates(){

	//Create result grid/
	if (markerSelector != NULL && videoReader !=NULL){
		/*	
		resultsGrid->ClearGrid();	//Clear the grid
		resultsGrid->CreateGrid(videoReader->getNumberOfIndices(),2*markerSelector->markers.size());
		std::vector<wxString> columnHeadings = std::vector<wxString>();
		//Column headings
		for (int m = 0; m<markerSelector->markers.size();++m){
				columnHeadings.push_back(markerSelector->GetString(m).Append(wxString(wxT(" X"))));
				columnHeadings.push_back(markerSelector->GetString(m).Append(wxString(wxT(" Y"))));
		}

		//Set Column labels
		for (int m = 0; m<columnHeadings.size();++m){
			//for (int c = 0;c<2;++c){
				resultsGrid->SetColLabelValue(m,columnHeadings[m]);
				//printf("%s\t%s\n",std::string(columnHeadings[m].mb_str()).c_str(),resultsGrid->GetColLabelValue(m).mb_str());
			//}
		}
		//Set row labels
		for (int f = 0; f<videoReader->getNumberOfIndices();++f){
			resultsGrid->SetRowLabelValue(f,wxString::Format(wxT("%i"),f));
		}
		*/
		
		//Get ready to print the results to file
		wxString resultLine = wxString(_(""));
		if (openSave != NULL){
			resultLine+=wxT("Frame #\t");
			for (int m = 0; m<markerSelector->markers.size();++m){
				resultLine+=markerSelector->GetString(m).Append(wxString(wxT(" X")));
				resultLine+=wxT("\t");
				if (m == markerSelector->markers.size()-1){	//Don't print tab after the last column
					resultLine+=markerSelector->GetString(m).Append(wxString(wxT(" Y")));
				}else{
					resultLine+=markerSelector->GetString(m).Append(wxString(wxT(" Y")));
					resultLine+=wxT("\t");
				}
			}
			//printf("Trying to clear openSave\n");
			openSave->Clear();
			//Write header line
			openSave->AddLine(resultLine);
		}
		
		
		//Print the data
		//Loop through frames
		//printf("Start printing to clear openSave\n");
		for (int i = 0; i<videoReader->getNumberOfIndices();++i){
			//Loop through markers
			resultLine = wxString::Format(wxT("%i"),i);
			resultLine+=_("\t");
			for (int m = 0; m<markerSelector->markers.size();++m){
				
				try{	
					coordinate markerCoordinate = markerSelector->getCoordinate(m, i);
					//resultsGrid->SetCellValue(i,m*2+0,wxString::Format(wxT("%f"),markerCoordinate.xCoordinate));
					//resultsGrid->SetCellValue(i,m*2+1,wxString::Format(wxT("%f"),markerCoordinate.yCoordinate));
					resultLine+=wxString::Format(wxT("%f"),markerCoordinate.xCoordinate);
					resultLine+=_("\t");
					resultLine+=wxString::Format(wxT("%f"),markerCoordinate.yCoordinate);
					
				} catch (...){	//Didn't have marker in current frame
					//Print NaN or -1 for missing
					//resultsGrid->SetCellValue(i,m*2+0,_("NaN"));
					//resultsGrid->SetCellValue(i,m*2+1,_("NaN"));
					resultLine+=_("NaN");
					resultLine+=_("\t");
					resultLine+=_("NaN");
				}
				if (m == markerSelector->markers.size()-1){ //Don't print tab after the last column
				
				}else{
					resultLine+=_("\t");
				}
			}
			if (openSave != NULL){
				openSave->AddLine(resultLine);
			}
		}
		if (openSave != NULL){
			//printf("Write to openSave\n");
			openSave->Write();
		}
	}
}	


/**Select marker from the drop down list*/
void DigitizerFrame::SelectMarker(wxCommandEvent &event){
	int selectedMarker = markerSelector->GetCurrentSelection();	/*Number of active marker*/
	markerSelector->currentMarker = selectedMarker;
	GetMarkerValues(selectedMarker);
}

/**Proceed to next marker*/
void DigitizerFrame::NextMarker(){
	int currentMarker = markerSelector->GetCurrentSelection();	/*Number of active marker*/
	if (currentMarker+1< (int) markerSelector->GetCount()){
		currentMarker+=1;
		markerSelector->SetSelection(currentMarker);
		markerSelector->currentMarker = currentMarker;
	}
	GetMarkerValues(currentMarker);
}

/**Back to previous marker*/
void DigitizerFrame::PreviousMarker(){
		int currentMarker = markerSelector->GetCurrentSelection();	/*Number of active marker*/
	if (currentMarker-1>=0){
		currentMarker-=1;
		markerSelector->SetSelection(currentMarker);
		markerSelector->currentMarker = currentMarker;
	}
	GetMarkerValues(currentMarker);
}

/**< Clear all coordinates*/
void DigitizerFrame::ClearMarker(wxCommandEvent &event){
	markerSelector->eraseMarker(markerSelector->currentMarker);
	redrawFrame();
}

/**< Clear coordinates from current frame onwards*/			
void DigitizerFrame::ClearOnwards(wxCommandEvent &event){
	for (int i = currentFrame;i<videoReader->frameIndices.size();++i){
		markerSelector->eraseCoordinate(markerSelector->currentMarker,i);
	}
	redrawFrame();
}			
	

void  DigitizerFrame::GetMarkerValues(int marker){
	int currentSearchRadius = markerSelector->markers.at(marker).searchRadius;	/*Save the slider value as the new marker radius*/
	int currentMarkerRadius = markerSelector->markers.at(marker).markerRadius;	/*Save the slider value as the new search radius*/
	int currentTolerance = markerSelector->markers.at(marker).colorTolerance;	/*Save the slider value as the new marker tolerance*/
	searchRadius->SetValue(currentSearchRadius);
	markerRadius->SetValue(currentMarkerRadius);
	colorTolerance->SetValue(currentTolerance);
}


/**Adjust marker size*/
void DigitizerFrame::AdjustSearchRadius(wxScrollEvent &event){
	//Set the values for the current marker
	int currentMarker = markerSelector->currentMarker;	/*Number of active marker*/
	int currentVal = searchRadius->GetValue();	/*Get the value from the slider*/
	markerSelector->markers.at(currentMarker).searchRadius =	currentVal;	/*Save the slider value as the new marker radius*/
	markerSelector->markers.at(currentMarker).searchCoordinates = markerSelector->getRelativeSamplingCoordinates((double) currentVal);
	SetStatusText(wxString::Format(wxT("%s %d"),_("# of search radius area coordinates"), markerSelector->markers.at(currentMarker).searchCoordinates->size()));
	
}



/**Adjust search radius*/
void DigitizerFrame::AdjustMarkerRadius(wxScrollEvent &event){
	//Set the values for the current marker
	int currentMarker = markerSelector->currentMarker;	/*Number of active marker*/
	int currentVal = markerRadius->GetValue();	/*Get the value from the slider*/
	markerSelector->markers.at(currentMarker).markerRadius =	currentVal;	/*Save the slider value as the new marker radius*/
	markerSelector->markers.at(currentMarker).radiusCoordinates = markerSelector->getRelativeSamplingCoordinates((double) currentVal);
	SetStatusText(wxString::Format(wxT("%s %d"),_("# of marker radius area coordinates"), markerSelector->markers.at(currentMarker).radiusCoordinates->size()));
}

/**Adjust region grow color tolerance*/
void DigitizerFrame::AdjustColorTolerance(wxScrollEvent &event){
	//Set the values for the current marker
	int currentMarker = markerSelector->currentMarker;	/*Number of active marker*/
	int currentVal = colorTolerance->GetValue();	/*Get the value from the slider*/
	markerSelector->markers.at(currentMarker).colorTolerance =	currentVal;	/*Save the slider value as the new marker radius*/
	SetStatusText(wxString::Format(wxT("%s %d"),_("Color tolerance set to"), markerSelector->markers.at(currentMarker).colorTolerance));
}

/**Turn auto-tracking on and off*/
void DigitizerFrame::ToggleTracking(wxCommandEvent &event){
	trackOn = toggleTrack->GetValue();
	/*Switch tracking on*/
	if (trackOn){
		trackingThread = new TrackingThread(this);
		trackingThread->startThread();
		//debugging
		//trackingThread->tThread.join();	/*Wait for the tracking thread to finish*/
		
	}else{ /*Stop the tracking thread*/
		/*Implement here*/
	}
}

/**Event table*/
BEGIN_EVENT_TABLE(DigitizerFrame, wxFrame)
	EVT_CLOSE(DigitizerFrame::OnQuit)
    EVT_MENU(ID_Quit,  		DigitizerFrame::OnQuit)
    EVT_MENU(ID_About, 		DigitizerFrame::OnAbout)
	EVT_COMMAND_SCROLL(ID_slider,	DigitizerFrame::ScrollVideo)
	EVT_COMMAND_SCROLL(ID_searchRadius,	DigitizerFrame::AdjustSearchRadius)	
	EVT_COMMAND_SCROLL(ID_markerRadius,	DigitizerFrame::AdjustMarkerRadius)
	EVT_COMMAND_SCROLL(ID_colorTolerance,	DigitizerFrame::AdjustColorTolerance)
	EVT_TOGGLEBUTTON(ID_toggleTracking, DigitizerFrame::ToggleTracking)
END_EVENT_TABLE()
