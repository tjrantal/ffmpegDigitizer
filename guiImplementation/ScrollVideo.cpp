void DigitizerFrame::ScrollVideo(wxScrollEvent &event){
	int currentVal = slider->GetValue();
	resultsText->ChangeValue(wxString::Format(wxT("%s %d"),"Frame #", currentVal));
	SetStatusText(wxString::Format(wxT("%s %d"),"Frame #", currentVal));
}