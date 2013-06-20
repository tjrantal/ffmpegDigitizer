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
	/*Slider for browsing video*/
	slider = new wxSlider(this,ID_slider,0,0,100,wxPoint(300,470),wxSize(400,40));
	//Connect(ID_slider,EVT_SCROLL,wxCommandEventHandler(DigitizerFrame::ScrollVideo),NULL,this);
	/*Connect button click events*/
	resultsText->Connect(wxEVT_LEFT_DOWN,wxMouseEventHandler(DigitizerFrame::LeftButtonDown), NULL,this);
	resultsText->Connect(wxEVT_LEFT_UP,wxMouseEventHandler(DigitizerFrame::LeftButtonUp),NULL,this);
	/*Connect events to the main frame*/
	Connect(wxEVT_LEFT_DOWN,wxMouseEventHandler(DigitizerFrame::LeftButtonDown), NULL,this);
	Connect(wxEVT_LEFT_UP,wxMouseEventHandler(DigitizerFrame::LeftButtonUp),NULL,this);
	
	imagePanel = new ImagePanel(this,_("DSC_0001.JPG"),wxPoint(200,10),wxSize(750,380));
	
}
