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
	
	//#include <windows.h>
	#include <vector>
	#include <stdio.h>	/*Check file size to determine whether the packets can be read to memory*/
	extern "C" {
		#include <libavcodec/avcodec.h>
		#include <libavformat/avformat.h>
		#include <libswscale/swscale.h>
		#include <libavutil/avutil.h>	/*Stream duration*/
		#include <libavutil/imgutils.h>	/*av_image_alloc*/
	}
	
	/**Struct to store frame indices and corresponding time stamps*/
	typedef struct{
		int 	frameNo;	/**< Number of picture in series*/
		int64_t	pts;	/**< TimeStamp of the frame */
		int64_t	pkt_pts;	/**< TimeStamp of the packet */
	} frameIndice;
	
	
	/*! CLASS FOR VIDEO READING*/
	class VideoReader
	{
		private:
		/*!Private vars*/
		AVCodecContext *pCodecCtx; /*!< Codec context*/
		AVFrame *tmp_picture;	/*!< Temp AVFrame*/
		AVFrame *picture;		/*!< swscaled AVFrame*/
		AVFormatContext *pFormatCtx;	/*!< Format context*/
		int videoStream;			/*!< video stream index*/
		AVOutputFormat *fmt;
		uint8_t *picture_buf[4];
		int		bufLinesize[4];
		int bufSize;
		uint8_t *picture_buf2[4];
		int		buf2Linesize[4];
		int bufSize2;
		struct SwsContext *img_convert_ctx;
		AVPacket packet;
		AVCodec         *pCodec;
		int             numBytes;
		int lastFrame;	/*Index of last decoded frame*/
	
		public:
		const char *filename; /**< Name of the video file*/
		int width;			/**< Width of the video frame*/
		int height;		/**< Height of the video frame*/
		double timeBase;	/**< ffmpeg timeBase*/
		double duration;	/**< ffmpeg duration of the file in s*/
		long startTime;		/**< ffmpeg startTime*/
		bool videoOpen;		/**< true, if video was opened successfully*/
		unsigned char *decodedFrame;	/**< Current decoded frame*/
		std::vector<frameIndice> frameIndices;	/**< frame indices and timestamps*/

		//Class function declarations
		int getNumberOfFrames();	/**< Get the number of frames in the video (might not work...)*/
		int readIndices();				/**< Read frame indices and timeStamps from disk, needs to be run first, othewise will miss any frames that have been read already */
		int readNextFrameFromDisk();	/**< Reads the next package in series */
		int readFrameFromDisk(int frameNo);	/**< Reads the @param frameNo frame from the file */
		int getNumberOfIndices();	/**< Returns the number of video indices*/
		//Functions declared
	
		/**Consructor,
		@param file file to open
		@param fram number of frames to read at a time
		*/
		VideoReader(const char* file); 
		
		/**Deconstructor*/
		~VideoReader();	/**< Close the video file and release memory*/
		
	};
#endif
