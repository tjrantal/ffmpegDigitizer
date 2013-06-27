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
	#endif
	#ifndef UINT64_C
		#define UINT64_C(c) (c ## ULL)
	#endif
	
	#include <windows.h>
	#include <vector>
	extern "C" {
		#include <libavcodec/avcodec.h>
		#include <libavformat/avformat.h>
		#include <libswscale/swscale.h>
		#include <libavutil/avutil.h>	/*Stream duration*/
		#include <libavutil/imgutils.h>	/*av_image_alloc*/
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
		uint8_t *picture_buf[4];
		int		bufLinesize[4];
		int bufSize;
		uint8_t *picture_buf2[4];
		int		buf2Linesize[4];
		int bufSize2;
		std::vector<int64_t> tstamp;
		struct SwsContext *img_convert_ctx;
		AVPacket packet;
		AVCodec         *pCodec;
		int             numBytes;
	
		public:
		const char *filename; /**< Name of the video file*/
		int frames;			/**< Number of frames read to memory*/
		int width;			/**< Width of the video frame*/
		int height;		/**< Height of the video frame*/
		int varattu;		/**< does nothing*/
		double timeBase;	/**< ffmpeg timeBase*/
		double duration;	/**< ffmpeg duration of the file in s*/
		long startTime;		/**< ffmpeg startTime*/
		bool videoOpen;		/**< true, if video was opened successfully*/
		unsigned char **video;	/**< Pointer to the frames read from the video*/

		//Class function declarations
		int readFrames();	/**< Read next "frames" frames or to the end of the video*/
		int getNumberOfFrames();	/**< Get the number of frames in the video (might not work...)*/
		//Functions declared
	
		/**Consructor,
		@param file file to open
		@param fram number of frames to read at a time
		*/
		VideoReader(const char* file, int fram); 
		
		/**Deconstructor*/
		~VideoReader();	/**< Close the video file and release memory*/
		
	};
#endif
