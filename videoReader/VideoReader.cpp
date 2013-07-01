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
	#include "VideoReader.h"
#endif
//Constructor
VideoReader::VideoReader(const char* file, int fram)
{
	/*Check file size, if less than 1G the file packages will be read into memory, otherwise nothing happens*/
	try{
		FILE* tempFile = fopen(file,"rb");
		fseek(tempFile,0,SEEK_END);
		fileSize = ftell(tempFile);
		fclose(tempFile);
		printf("Got filesize %d MB\n",(int) fileSize/1024/1024);
		
	}catch (int e){
		printf("Couldn't get file size\n");
	}
	
	videoOpen = false;
	av_register_all();	//Register formats
	filename = file;		//File to open
	frames = fram;	//Number of frames to be read
	videoFrames = frames;
	varattu = frames;
	for (int lll = 0;lll<2;++lll){
		tstamp.push_back(0);
	}
	
	// Open video file
	pFormatCtx = NULL;
	printf("%s\n",filename);
	fflush(stdout);			//DEBUGGING
	if(avformat_open_input(&pFormatCtx, filename, NULL, NULL)!=0){
		printf("Couldn't open file\n");
		fflush(stdout);			//DEBUGGING
		return;
	}
				//return -1; // Couldn't open file
	// Retrieve stream information
	printf("findStreams\n");
	fflush(stdout);			//DEBUGGING
	if(avformat_find_stream_info(pFormatCtx,NULL)<0){
		printf("Coudln't find stream\n");
		fflush(stdout);			//DEBUGGING
		return;
	}

	av_dump_format(pFormatCtx, 0, filename, 0);
	fflush(stdout);			//DEBUGGING
			// Find the first video stream
	
	  /* retrieve stream information */
	videoStream=-1;
	for(unsigned int i=0; i<pFormatCtx->nb_streams; i++){
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
		{
		videoStream=i;
		break;
		}
	}
	if(videoStream==-1){
		printf("no videostream\n");
		fflush(stdout);			//DEBUGGING
		return;
	}
	printf("find_stream_info\n");
	fflush(stdout);			//DEBUGGING
	
	videoStream = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	//Look for videostream index and open codec context
	if (videoStream < 0) {
	    printf("Could not videoStreamIndex\n");
		fflush(stdout);			//DEBUGGING
        return;
	}	
		printf("videoStreamIndex\n");
	fflush(stdout);			//DEBUGGING
	// Get a pointer to the codec context for the video stream
	pCodecCtx=pFormatCtx->streams[videoStream]->codec;
	width = pCodecCtx->width;
	height = pCodecCtx->height;

		// Find the decoder for the video stream
	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec==NULL){
		printf("No suitable codec\n");
		return;
		}
	// Open codec
	if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
		printf("Couldn't open codec\n");
		return;
		}
		// Hack to correct wrong frame rates that seem to be generated by some codecs
	if(pCodecCtx->time_base.num>1000 && pCodecCtx->time_base.den==1)
		pCodecCtx->time_base.den=1000;
	


	
	tmp_picture=avcodec_alloc_frame();
	if (!tmp_picture){
		printf("Coulnd't alloc frame\n");
		fflush(stdout);			//DEBUGGING
		return;
	}
	
	//Reserve memory for frames
	printf("Reserve Memory\n");
	fflush(stdout);			//DEBUGGING

	img_convert_ctx = NULL;
	/*PIX_FMT_YUV420P*/
	if (pCodecCtx->pix_fmt != PIX_FMT_RGB24) {
		printf("different pix_fmt\n");
	
		picture = avcodec_alloc_frame();
		if (!picture){
			printf("Couldn't alloc frame\n");
			return;
		}

		int ret = av_image_alloc(picture->data, picture->linesize,
						 pCodecCtx->width, pCodecCtx->height,
						 PIX_FMT_RGB24, 1);
		if (ret < 0) {
			fprintf(stderr, "Could not allocate raw video buffer\n");
			return;
		}
		
		/* as ffmpeg returns a YUV420P picture from a video, we must convert it
		   to the desired pixel format */
		if (img_convert_ctx == NULL) {
			/*Target image format PIX_FMT_RGB24*/
			img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
											 pCodecCtx->pix_fmt,
											 width, height,
											 PIX_FMT_RGB24
											 , SWS_BICUBIC, NULL, NULL, NULL);
			if (img_convert_ctx == NULL) {
				fprintf(stderr, "Cannot initialize the conversion context\n");
				return;
			}
		}
	}
	videoOpen = true;
	av_init_packet(&packet);
	packet.data = NULL;
	packet.size = 0;
	//Reserve memory for a frame
	decodedFrame = new unsigned char [width*height*3*sizeof(unsigned char)];
}

/*Read all of the packets to memory, might need to check whether the video is small enough to it into memory..*/
int VideoReader::readPackets(){
	int frameFinished;
	int lastKeyFramePacket = 0;
	packets = std::vector<framePacket>();
	int currentPacket = 0;
	int readMore = 1;
	while(1) /*Read all frames to memory*/
	{
		if (av_read_frame(pFormatCtx, &packet)<0){
			break;
		}
		AVPacket tempPacket;
		av_init_packet(&tempPacket);
		tempPacket.data = NULL;
		tempPacket.size = 0;

	    if(packet.stream_index==videoStream && av_read_frame(pFormatCtx, &tempPacket)>=0)		 // Is this a packet from the video stream?
	    {	
			avcodec_decode_video2(pCodecCtx, tmp_picture, &frameFinished, 
	            &tempPacket);
			if (tmp_picture->key_frame == 1){
				lastKeyFramePacket = currentPacket;
			}
			framePacket lastPacket = {tempPacket,lastKeyFramePacket,tmp_picture->display_picture_number};
			packets.push_back(lastPacket);
			++currentPacket;
	    }
	}
	lastPacket = -1;	/*Set last packet to -1, since none have been decoded*/
	lastFrame = -1;		/*Set last frame to -1, since none have been decoded*/
	return 1;

}

/* Decodes next packet from <framePacket> packets*/
int VideoReader::decodeNextFrame(){
	int frameFinished = 0;
	while (!frameFinished){
		if (packets.size()< lastPacket+1){
			printf("No more packets, packet %d\n",lastPacket);
			return 0;
		}
		avcodec_decode_video2(pCodecCtx, tmp_picture, &frameFinished, &packets.at(lastPacket+1).packet);            // Decode video frame
		++lastPacket;
		if(frameFinished)	            // Did we get a video frame?
		{
			if(img_convert_ctx == NULL){
				/*No conversion necessary*/
				if (tmp_picture->linesize[0] != width || tmp_picture->linesize[0] != width*3){ //Hack for padding
					for (int zzz = 0; zzz < height;zzz++){
						memcpy(decodedFrame+zzz*sizeof(unsigned char)*width*3,tmp_picture->data[0]+zzz*tmp_picture->linesize[0],width*sizeof(unsigned char)*3);
					}
				} else {
					memcpy(decodedFrame,tmp_picture->data[0],width*height*sizeof(unsigned char)*3);
				}
			}else{//If pixel_fmt is not targetFormat
				sws_scale(img_convert_ctx, tmp_picture->data, tmp_picture->linesize,
				  0, height, picture->data, picture->linesize);
		
				if (picture->linesize[0] != width && picture->linesize[0] != width*3){//Hack for padding (probably not needed...
					printf("%d memcpy hack\n",picture->linesize[0]);
					for (int zzz = 0; zzz < height;zzz++){
						memcpy(decodedFrame+zzz*sizeof(unsigned char)*width*3,picture->data[0]+zzz*tmp_picture->linesize[0],width*sizeof(unsigned char)*3);
					}
				} else {
					memcpy(decodedFrame,picture->data[0],width*height*sizeof(unsigned char)*3);
				}
			}
		}
	}
	lastFrame = tmp_picture->display_picture_number;
	printf("Frame %d\n",lastFrame);
	fflush(stdout);
	return 1;
}

/* Decodes frame frameNo from <framePacket> packets*/
int VideoReader::decodeFrame(int frameNo){
	/*If next in order, just decode the next frame and return*/
	if (frameNo == lastFrame+1){
		decodeNextFrame();
		return 1;
	}
	lastPacket = packets.at(frameNo).lastKeyframePacket-1;
	int frameFinished = 0;
	int currentFrame = -1;
	
	
	while (!frameFinished && currentFrame!= frameNo){
		if (packets.size()< lastPacket+1){
			return 0;
		}
		avcodec_decode_video2(pCodecCtx, tmp_picture, &frameFinished, &packets.at(lastPacket+1).packet);            // Decode video frame
		++lastPacket;
		if(frameFinished)	            // Did we get a video frame?
		{
			currentFrame = tmp_picture->display_picture_number;
		}
	}
	/*Convert the image*/
	if(img_convert_ctx == NULL){
		/*No conversion necessary*/
		if (tmp_picture->linesize[0] != width || tmp_picture->linesize[0] != width*3){ //Hack for padding
			for (int zzz = 0; zzz < height;zzz++){
				memcpy(decodedFrame+zzz*sizeof(unsigned char)*width*3,tmp_picture->data[0]+zzz*tmp_picture->linesize[0],width*sizeof(unsigned char)*3);
			}
		} else {
			memcpy(decodedFrame,tmp_picture->data[0],width*height*sizeof(unsigned char)*3);
		}
	}else{//If pixel_fmt is not targetFormat
		sws_scale(img_convert_ctx, tmp_picture->data, tmp_picture->linesize,
		  0, height, picture->data, picture->linesize);

		if (picture->linesize[0] != width && picture->linesize[0] != width*3){//Hack for padding (probably not needed...
			printf("%d memcpy hack\n",picture->linesize[0]);
			for (int zzz = 0; zzz < height;zzz++){
				memcpy(decodedFrame+zzz*sizeof(unsigned char)*width*3,picture->data[0]+zzz*tmp_picture->linesize[0],width*sizeof(unsigned char)*3);
			}
		} else {
			memcpy(decodedFrame,picture->data[0],width*height*sizeof(unsigned char)*3);
		}
	}
	lastFrame = tmp_picture->display_picture_number;
	return 1;
}	

int VideoReader::readFrames(){

	int frameja2 = 0;
	int frameFinished;

	if (tstamp[0] != 0){ //Have to seek in order to start from the last key frame prior to the time stamp of interest
		printf("\nSeek\n");
		int64_t targetPosition = tstamp[2];
		printf("Tstamp prior to search %ld\n", (long)tstamp[0]);
		int success = av_seek_frame(pFormatCtx,videoStream,tstamp[2],AVSEEK_FLAG_BACKWARD);
		if (success < 0){
			printf("Seek failes %d\n",success);
			return 1;
		}
		bool moreFrames = true;
		while (moreFrames){
			av_read_frame( pFormatCtx, &packet );
			if(packet.stream_index==videoStream)
			{
				avcodec_decode_video2(pCodecCtx, tmp_picture, &frameFinished, 
	            &packet);
				targetPosition = packet.pts;
				if (targetPosition == tstamp[0]){
					moreFrames = false;
				}
			}
		
			av_free_packet( &packet );
		}
		 printf("Tstamp after seek %ld packet %ld\n",(long)targetPosition,(long)packet.pts);
	}

	while(av_read_frame(pFormatCtx, &packet)>=0 && frameja2 < frames )
	{
	    if(packet.stream_index==videoStream)		 // Is this a packet from the video stream?
	    {
	        avcodec_decode_video2(pCodecCtx, tmp_picture, &frameFinished, &packet);            // Decode video frame
	        if(frameFinished)	            // Did we get a video frame?
	        {
				if(img_convert_ctx == NULL){
					if (tmp_picture->linesize[0] != width){ //Hack for padding
						for (int zzz = 0; zzz < height;zzz++){
							memcpy(video[frameja2]+zzz*width*3,tmp_picture->data[0]+zzz*tmp_picture->linesize[0]*3,width*sizeof(unsigned char)*3);
						}
					} else {
						memcpy(video[frameja2],tmp_picture->data[0],width*height*sizeof(unsigned char)*3);
					}
				}else{//If pixel_fmt is not targetFormat
					sws_scale(img_convert_ctx, tmp_picture->data, tmp_picture->linesize,
	                  0, height, picture->data, picture->linesize);
			
					if (picture->linesize[0] != width && picture->linesize[0] != width*3){//Hack for padding (probably not needed...
						printf("%d memcpy hack\n",picture->linesize[0]);
						for (int zzz = 0; zzz < height;zzz++){
							memcpy(video[frameja2]+zzz*width*3,picture->data[0]+zzz*tmp_picture->linesize[0]*3,width*sizeof(unsigned char)*3);
						}
					} else {
						memcpy(video[frameja2],picture->data[0],width*height*sizeof(unsigned char)*3);
					}
				}
				frameja2++;
				tstamp.insert(tstamp.begin(),packet.pts);
				//printf("tStamp %ld",(long) packet.pts);
				tstamp.pop_back();
			}
	    }
	    // Free the packet that was allocated by av_read_frame
	    av_free_packet(&packet);
	}
	frames = frameja2;
	return 1;
}

VideoReader::~VideoReader(){
	// Close the video file
	printf("Closing avcodec\n");		//DEBUGGING
	fflush(stdout);			//DEBUGGING
	avcodec_close(pCodecCtx);
	printf("\nClosing avformat\n");	//DEBUGGING
	fflush(stdout);			//DEBUGGING
	avformat_close_input(&pFormatCtx);
	printf("Set format and codec to null\n");		//DEBUGGING
	fflush(stdout);			//DEBUGGING
	pFormatCtx = NULL;
	pCodecCtx =NULL;
	
	//Attempt to free pictures
	printf("free tmp_picture\n");	//DEBUGGING
	fflush(stdout);			//DEBUGGING AVFrame
	avcodec_free_frame(&tmp_picture);
	//av_free(&tmp_picture);
	
	/*
	AVFrame* tempPointer = (AVFrame*) &tmp_picture;
	 av_frame_free(&tempPointer);
	 */

	//av_freep(&picture_buf[0]);
	printf("free picture->data\n");	//DEBUGGING
	fflush(stdout);			//DEBUGGING
	av_freep(&picture->data[0]);
	printf("free picture\n");	//DEBUGGING
	fflush(stdout);			//DEBUGGING
	avcodec_free_frame(&picture);
	printf("free  buffer\n");	//DEBUGGING
	fflush(stdout);			//DEBUGGING
	//av_free(&picture_buf[0]);
	/*
	tempPointer = (AVFrame*) &picture;
	 av_frame_free(&tempPointer);
	 */
	//Free memory
	
	printf("free memory\n");	//DEBUGGING
	fflush(stdout);			//DEBUGGING

	if (&packets != NULL){
		packets.clear();
	}
	printf("packets cleared\n");	//DEBUGGING
	fflush(stdout);			//DEBUGGING
}

int VideoReader::getNumberOfFrames(){
	timeBase= (double)pFormatCtx->streams[videoStream]->time_base.num/(double)pFormatCtx->streams[videoStream]->time_base.den;
	duration = ((double) pFormatCtx->streams[videoStream]->duration)*timeBase;
	startTime=(long) pFormatCtx->streams[videoStream]->start_time;
	double frameInterval = (double)pFormatCtx->streams[videoStream]->codec->time_base.num/(double)pFormatCtx->streams[videoStream]->codec->time_base.den;
	printf("startTime %ld duration %.2f timeBase %.8f frameInterval %.4f\n",startTime,(float) duration,(float) timeBase,(float) frameInterval);
	if (pFormatCtx->streams[videoStream]->nb_frames > 0){
	
		printf("Stream returned the number of frames %d\n",(int) pFormatCtx->streams[videoStream]->nb_frames);
		return (int) pFormatCtx->streams[videoStream]->nb_frames;
	}
	printf("Stream DID NOT return the number of frames %d\n",(int) (duration/frameInterval));

	return (int) (duration/timeBase);
}

int VideoReader::getNumberOfPackets(){
	return packets.size();
}