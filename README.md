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


ffmpegDigitizer
===============

A program for digitizing marker trajectories from video. I am working on an open source pipeline to go from several simultaneously captured videos to 3D-coordinates of markers placed on an object of interest (i.e. 3D motion capture).

The plan is to:

	Write digitizer for 2D-images:
		Read text file with marker names
		Read video (ffmpeg)
		Show the first frame (wxWidgets)
		Add slider to browse through the video
		Digitize all markers
			Start automatic tracking using just RGB thresholds and sufficient number of suitable pixels in a circular region (select the best match, with a certain minimum match goodness)
			Track all markers simultaneously, allow stopping the autotrack
			Stop if marker is lost, ask to digitize the marker manually (allow disabling the marker)
		
	WxWidgets used for GUI - needs to be compiled in the MSYS environment...
		
	Do separate camera calibration in matlab, M‰enp‰‰'s Octave/Matlab code (http://www.cse.oulu.fi/CMV/Downloads/CameraCalibrationToolboxForMatlab) using standard calibration object
	
	Apply the distortion correction and use standard DLT to get marker 3D trajectories with Octave/Matlab
