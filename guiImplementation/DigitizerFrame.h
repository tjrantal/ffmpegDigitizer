class DigitizerFrame: public wxFrame
{
public:
    DigitizerFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:
	//Variables
	wxTextCtrl		*resultsText;			//For displaying text in screen
	clock_t			*clockCounts,initClock;	//For storing processor times
	wxButton		*resetButton;			//For resetting the clock
	float			elapsedTime;				//time since first click
	bool			timeRunning;				//to indicate whether the first click has passed
	FILE			*resultFile;			//File to save to
	wxButton 		*openMarkerFile;		//Pick a file to save to
	wxFile 			*openFile;
	wxSlider		*slider;				//Slider
    void OnQuit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
	void LeftButtonDown(wxMouseEvent &event);
	void LeftButtonUp(wxMouseEvent &event);
	void ResetClock(wxCommandEvent &event);
	void OpenFile(wxCommandEvent &event);
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
	ID_slider,
};

#include "DigitizerFrame.cpp"
#include "OpenFile.cpp"
#include "ScrollVideo.cpp"
#include "OnQuit.cpp"
#include "OnAbout.cpp"
#include "LeftButtonDown.cpp"
#include "LeftButtonUp.cpp"
