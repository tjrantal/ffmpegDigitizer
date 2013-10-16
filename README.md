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
	gsl	(http://www.gnu.org/software/gsl/)		for marker tracking	 (not needed 2.7.2013)
	obviously any dependencies the above have
	
	Has to be compiled with a ISO C++ 2011 standard supporting compiler (e.g. Gnu C++ compiler version 4.7 or higher) due to threading using pthreads.
	
The plan is to:

	Write digitizer for 2D-images:
		Read text file with marker names		(implemented 2.7.2013)
		Read video (ffmpeg)						(implemented 2.7.2013)
		Show the first frame (wxWidgets)		(implemented 2.7.2013)
		Add slider to browse through the video	(implemented 2.7.2013)
[TO DO]
		Digitize all markers
			Start automatic tracking using just RGB thresholds and sufficient number of suitable pixels in a circular region (select the best match, with a certain minimum match goodness)
				Give each marker a marker radius and a search radius. Using sliders for that...
			Track all markers simultaneously, allow stopping the autotrack
			Stop if marker is lost, ask to digitize the marker manually (allow disabling the marker)
		
	WxWidgets used for GUI - needs to be compiled in the MSYS environment...
		
	Do separate camera calibration in matlab, Mäenpää's Octave/Matlab code (http://www.cse.oulu.fi/CMV/Downloads/CameraCalibrationToolboxForMatlab) using standard calibration object
	
	Apply the distortion correction and use standard DLT to get marker 3D trajectories with Octave/Matlab

NOTES I made on  my latest setting up of my Windows development environment at Deakin Uni

MINGW64 and MSYS
download mingw-w64 + mingw-w64 msys, the most inclusive versions, so you also get package config
mingw-w64 http://mingw-w64.sourceforge.net/download.php#mingw-builds
msys http://sourceforge.net/apps/trac/mingw-w64/wiki/MSYS select the mingw-builds version

copy 64-bit version of yasm (http://yasm.tortall.net/Download.html) to your mingw64/bin, rename it to yasm.exe

Add library path and pkg-config path to your ~/.profile so that pkg-config works properly
export LIBRARY_PATH=/mingw/lib
export PKG_CONFIG_PATH=/mingw/lib/pkgconfig
export CPATH=/mingw/include

Compile and Install x264 codec
./configure --prefix=/mingw --enable-static --host=x86_64-w64-mingw32
make
make install

Compile and Install lib SDL 
./configure --prefix=/mingw --enable-static --build=x86_64-w64-mingw32
make
make install

Compile and Install libFAAC (mp4v2 will not work in windows for some reason)
./configure --prefix=/mingw --with-mp4v2=no --enable-static --build=x86_64-w64-mingw32 
make
make install

Compile and Install gnu scientific library (gsl)
./configure --prefix=/mingw --enable-static --build=x86_64-w64-mingw32 
make
make install

FFMPEG (depends on libsdl and libfaac)
#FFMPEG configure tries to execute a file from the /tmp, which does not work in university network -> direct TEMP to some other place, where executing is allowed..
export TEMP=/home/tjrantal/TEMP
export TMP=$TEMP

./configure --enable-memalign-hack --enable-libfaac --enable-nonfree --enable-gpl --enable-libx264 --prefix=/mingw --extra-ldflags='-L/mingw/lib' --extra-cflags=-I/mingw/include --arch=x86_64
make
make install

Test wether ffmpeg works with the decoding_encoding example
gcc decoding_encoding.c -o decoding_encoding -static -IffmpegDev32\include -LffmpegDev32\lib -lavformat -lavcodec -lavutil -lswscale -lwsock32 -lgdi32 -lmingw32


Configure git checkout line feed (forgot to pick up the webpage with the instruction)
http://ffmpeg.org/trac/ffmpeg/wiki/MingwCompilationGuide


wxWidgets (use --build type) Downloaded 2.8.12 version
applied patch from http://code.google.com/p/mingw-w64-dgn/source/browse/trunk/patch/wxWidgets-2.8.12-w64.patch?r=146
(download the file as raw file from the right side of the page -> tabs etc. need to be preserved
apply the patch from msys with: patch -p0 < path/to/patchfile)
 
./configure --disable-shared --disable-debug --prefix=/mingw --build=x86_64-w64-mingw32
make 
make install
