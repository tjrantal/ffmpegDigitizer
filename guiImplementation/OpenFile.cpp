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
