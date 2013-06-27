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
#ifndef VIDEOREADER_H
#define VIDEOREADER_H
	/*Workaround libavformat "bug" with c++*/
	#ifndef INT64_C
	#define INT64_C(c) (c ## LL)
	#define UINT64_C(c) (c ## ULL)
	#endif
	//#include <windows.h>
	#include <vector>
	extern "C" {
		#include <libavcodec/avcodec.h>
		#include <libavformat/avformat.h>
		#include <libswscale/swscale.h>
		#include <libavutil/avutil.h>	/*Stream duration*/
	}

	/*! CLASS FOR VIDEO READING*/
	class VideoReader
	{
		private:
		/*!Private vars*/
		AVCodecContext *pCodecCtx; /*!< Codec context*/
		AVFrame tmp_picture;	/*!< Temp AVFrame*/
		AVFrame picture;		/*!< swscaled AVFrame*/
		AVFormatContext *pFormatCtx;	/*!< Format context*/
		int videoStream;			/*!< video stream index*/
		AVOutputFormat *fmt;
		uint8_t *picture_buf;
		uint8_t *picture_buf2;
		std::vector<int64_t> tstamp;
		struct SwsContext *img_convert_ctx;
		AVPacket packet;
		AVCodec         *pCodec;
		int             numBytes;
	
		public:
		const char *filename; /*!< Name of the video file*/
		int frames,leveys,korkeus,varattu;
		double timeBase,duration;
		long startTime;
		unsigned char **video;

		//Class function declarations
		int readFrames();
		int closeVideo();
		int getNumberOfFrames();
		//Functions declared
	
		/*Consructor,
		file name to open
		number of frames to read at a time
		*/
		VideoReader(const char* file, int fram);
	};
#endif
