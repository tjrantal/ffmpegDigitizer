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
#ifndef MARKERSELECTOR_H
#define MARKERSELECTOR_H

#include <wx/combobox.h>
#include <wx/wxfile.h>
#include <wx/string.h>
#ifndef DIGITIZERFRAME_H
	#include "../digitizerFrame/DigitizerFrame.h"
#endif

/*! CLASS FOR READING MARKERS FROM A FILE AND DISPLAYING A MARKER LIST*/
class MarkerSelector : public wxComboBox{
	private:
	
	public:
		/*Functions*/
	
		/**Consructor,
		@param file file to open
		@param fram number of frames to read at a time
		*/
		MarkerSelector(wxString fileIn, DigitizerFrame parent);
		
		/**Deconstructor*/
		~MarkerSelector();	/**< release memory*/

};

#endif