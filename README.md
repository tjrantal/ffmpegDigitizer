Copyright (C) 2013 - 2016  Timo Rantalainen (tjrantal at gmail dot com)

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

A program for digitizing marker trajectories from video. N.B. crashes randomly at the moment.

Dependencies
	ffmpeg (https://ffmpeg.org/)					for working with video files, used version 2.8.2 in Ubuntu
	wxWidgets (http://www.wxwidgets.org/)		for GUI, used version 3.0.2 in Ubuntu
	opencv	(http://opencv.org/)
	opencvContrib (https://github.com/Itseez/opencv_contrib)
	
	Has to be compiled with a ISO C++ 2011 standard supporting compiler (e.g. Gnu C++ compiler version 4.7 or higher) due to threading using pthreads. Use the cmake file for building.

WIN 7 compilation notes
wxWidgets 3.1.0 works. Compiled as a shared library (docs/msw/install.txt had details on compilation)
ffmpeg	downloaded Zeranoe pre-compiled -dev, shared runtime libs, unzipped the .dlls from the shared runtime to -dev lib
wxWidgets compilation whinged about tiff_config re-defining snprintf, commented that row out, and compiled after
Add FFMPEG_HOME, wxWidgets_ROOT_DIR, and wxWidgets_LIB_DIR environment vars. Look into cmake find wxWidgets for tips.

OpenCV with openCVContrib. Download both sources from git. In CMake configuration, add -DOPENCV_EXTRA_MODULES_PATH=<opencv_contrib>/modules to get contrib included
Add OPENCV_DIR=D:\timo\cppLibs\ocv64Build to environment vars, add %OPENCV_DIR%\bin\Release to path
