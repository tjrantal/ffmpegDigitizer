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

Dependencies (I may or may not update this list)
	ffmpeg (https://ffmpeg.org/)				for working with video files
	wxWidgets (http://www.wxwidgets.org/)		for GUI
	gsl	(http://www.gnu.org/software/gsl/)		for marker tracking
	obviously any dependencies the above have
	
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
		
	Do separate camera calibration in matlab, M�enp��'s Octave/Matlab code (http://www.cse.oulu.fi/CMV/Downloads/CameraCalibrationToolboxForMatlab) using standard calibration object
	
	Apply the distortion correction and use standard DLT to get marker 3D trajectories with Octave/Matlab

	
NOTES I made when I was settin up my developement environment on a Windows machine

Setting up the dev environment:
Get MinGW from here http://sourceforge.net/projects/mingw-w64/files/
Get MSYS from here http://sourceforge.net/apps/trac/mingw-w64/wiki/MSYS

Add minGW/bin to path

Get pkg-config http://www.gtk.org/download/win32.php

Install lib SDL and x264 (for ffplay and x264 codec..)

Install libFAAC http://www.audiocoding.com/downloads.html
modify the Makefile.. http://kemovitra.blogspot.fi/2009/08/mingw-to-compile-ffmpeg.html and run bootstrap (requires automake and libtool, just install from mingw/msys external)

libvpx
./configure --prefix=/libvpx --target=x86-win32-gcc
CFLAGS="-fno-tree-vectorize" ./configure --prefix=/libvpx --target=x86-win32-gcc --cpu=i686

wxWidgets
make requires the flag CXXFLAGS="-fno-keep-inline-dllexport", i.e.
make CXXFLAGS="-fno-keep-inline-dllexport"


Mingw ffmpeg compilation guide
http://ffmpeg.org/trac/ffmpeg/wiki/MingwCompilationGuide
#FFMPEG configure tries to execute a file from the /tmp, which does not work in university network -> direct TEMP to some other place, where executing is allowed..
export TEMP=/home/tjrantal/TEMP
export TMP=$TEMP
/c/MyTemp/oma/Timon/tyo/AquaLoading2012/ffmpegDigitizer/src/ffmpeg/configure --enable-memalign-hack --enable-libfaac --enable-nonfree --enable-gpl --enable-libx264 --prefix=/ffmpeg --cpu=i686

Test wether ffmpeg works with the decoding_encoding example
gcc decoding_encoding.c -o decoding_encoding -static -IffmpegDev32\include -LffmpegDev32\lib -lavformat -lavcodec -lavutil -lswscale -lwsock32 -lgdi32 -lmingw32
