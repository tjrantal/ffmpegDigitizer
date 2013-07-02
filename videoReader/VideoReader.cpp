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

/*Read all of the packets to memory, might need to check whether the video is small enough to fit into memory..*/
int VideoReader::readIndices(){
	int frameFinished;
	frameIndices = std::vector<frameIndice>();
	lastFrame = -1;		/*Set last frame to -1, since none have been decoded*/
	int frameNo = -1;
	printf("\n");	
	while(av_read_frame(pFormatCtx, &packet)>=0) /*Read all frames to memory*/
	{
		
	    if(packet.stream_index==videoStream)		 // Is this a packet from the video stream?
	    {	
			avcodec_decode_video2(pCodecCtx, tmp_picture, &frameFinished, 
	            &packet);
	        if (frameFinished){
				++frameNo;
				frameIndice lastIndice = {frameNo,tmp_picture->pts,tmp_picture->pkt_pts};
				printf("%d %ld %ld\n",frameNo,(long int) lastIndice.pts,(long int) lastIndice.pkt_pts);
				frameIndices.push_back(lastIndice);
				av_free_packet(&packet);
			}
	    }else{
	    	av_free_packet(&packet);
	    }
	}
	/*Check whether null frames need to be fed in to get the remaining data*/
	printf("Lisalehdykoille %d\n",frameNo);
	fflush(stdout);
	while (1){
		packet.data = NULL;
		packet.size = 0;
		if (avcodec_decode_video2(pCodecCtx, tmp_picture, &frameFinished, 
	            &packet) >=0){
		
			if (frameFinished){
				++frameNo;
				frameIndice lastIndice = {frameNo,tmp_picture->pts,tmp_picture->pkt_pts};
				frameIndices.push_back(lastIndice);
				av_free_packet(&packet);
				printf("Lisalehdilla %d  %ld %ld\n",frameNo,(long int) lastIndice.pts,(long int) lastIndice.pkt_pts);
				fflush(stdout);
			}else{
				printf("Didn't get a frame anymore %d\n",frameNo);
				break;				
			}
			
		}else{
			break;
		}
	}
	
	
	av_seek_frame(pFormatCtx,videoStream,frameIndices.at(0).pts,AVSEEK_FLAG_BACKWARD); /*Rewind the file*/
	return frameNo;

}


int VideoReader::readNextFrameFromDisk(){
	printf("In reader\n");
	fflush(stdout);			//DEBUGGING
	int frameFinished = 0;
	int readMore = 1;
	while(readMore ==1 && frameFinished==0) //C(pFormatCtx, &packet)>=0
	{
		int readReturn = av_read_frame(pFormatCtx, &packet);
		printf("In while %d\n",readReturn); /*Is the stream open?*/
		fflush(stdout);			//DEBUGGING
		//Already at the end of file
		if (readReturn <0){
		 break;
		}
	    if(packet.stream_index==videoStream)		 // Is this a packet from the video stream?
	    {
	        avcodec_decode_video2(pCodecCtx, tmp_picture, &frameFinished, &packet);            // Decode video frame
				printf("Decoded %d\n",frameFinished);
						fflush(stdout);			//DEBUGGING
	        if(frameFinished)	            // Did we get a video frame?
	        {
				readMore = 0;
				printf("Decoded %d tStamp %ld pTStap %ld codedPNo %d\n",tmp_picture->display_picture_number, (long int) tmp_picture->pts,(long int) tmp_picture->pkt_pts,tmp_picture->coded_picture_number);
						fflush(stdout);			//DEBUGGING
				
				if(img_convert_ctx == NULL){
					if (tmp_picture->linesize[0] != width){ //Hack for padding
						for (int zzz = 0; zzz < height;zzz++){
							memcpy(decodedFrame+zzz*width*3,tmp_picture->data[0]+zzz*tmp_picture->linesize[0]*3,width*sizeof(unsigned char)*3);
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
							memcpy(decodedFrame+zzz*width*3,picture->data[0]+zzz*tmp_picture->linesize[0]*3,width*sizeof(unsigned char)*3);
						}
					} else {
						memcpy(decodedFrame,picture->data[0],width*height*sizeof(unsigned char)*3);
						
					}
				}
			}
	    }
	    // Free the packet that was allocated by av_read_frame
	    av_free_packet(&packet);
	}
	printf("Null frames? %d\n",readMore);
	/*Feed null frames need to get a delayed frame*/
	if (readMore ==1){
		printf("Going for a delayed frame %d\n",lastFrame+1);
		fflush(stdout);
		while (1){
			packet.data = NULL;
			packet.size = 0;
			if (avcodec_decode_video2(pCodecCtx, tmp_picture, &frameFinished, 
					&packet) >=0){
			
				if (frameFinished){
					if(img_convert_ctx == NULL){
						if (tmp_picture->linesize[0] != width){ //Hack for padding
							for (int zzz = 0; zzz < height;zzz++){
								memcpy(decodedFrame+zzz*width*3,tmp_picture->data[0]+zzz*tmp_picture->linesize[0]*3,width*sizeof(unsigned char)*3);
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
								memcpy(decodedFrame+zzz*width*3,picture->data[0]+zzz*tmp_picture->linesize[0]*3,width*sizeof(unsigned char)*3);
							}
						} else {
							memcpy(decodedFrame,picture->data[0],width*height*sizeof(unsigned char)*3);
							
						}
					}
					printf("Got a delayed Frame\n");
					fflush(stdout);
					break;
				}else{
					printf("Didn't get a frame anymore %d\n",lastFrame+1);
					fflush(stdout);
					break;				
				}
				
			}else{
				break;
			}
		}
	}
	
	
	++lastFrame;
	return lastFrame;
}

/** Reads the @param frameNo frame from the file */
int VideoReader::readFrameFromDisk(int frameNo){
	/*Do nothing, if requested frame = current frame*/
	if (frameNo == lastFrame){
		return frameNo;	
	}
	/*Read next frame*/
	if (frameNo == lastFrame+1){
		readNextFrameFromDisk();	
		lastFrame = frameNo;
		return frameNo;
	}
	/*Seek to the desired frame*/
	int success = av_seek_frame(pFormatCtx,videoStream,frameIndices.at(frameNo).pkt_pts,AVSEEK_FLAG_BACKWARD);
	if (success < 0){
		printf("Seek failed %d\n",success);
		fflush(stdout);
		return 0;
	}
	bool moreFrames = true;
	int frameFinished;
	int readReturn;
	while (moreFrames){
		readReturn = av_read_frame( pFormatCtx, &packet );
		//Already at the end of file
		if (readReturn <0){
		 break;
		}
		if(packet.stream_index==videoStream)
		{
			avcodec_decode_video2(pCodecCtx, tmp_picture, &frameFinished, 
			&packet);
			if (frameFinished){
				if (packet.pts == frameIndices.at(frameNo).pkt_pts){
					moreFrames = false;
					if(img_convert_ctx == NULL){
						if (tmp_picture->linesize[0] != width){ //Hack for padding
							for (int zzz = 0; zzz < height;zzz++){
								memcpy(decodedFrame+zzz*width*3,tmp_picture->data[0]+zzz*tmp_picture->linesize[0]*3,width*sizeof(unsigned char)*3);
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
								memcpy(decodedFrame+zzz*width*3,picture->data[0]+zzz*tmp_picture->linesize[0]*3,width*sizeof(unsigned char)*3);
							}
						} else {
							memcpy(decodedFrame,picture->data[0],width*height*sizeof(unsigned char)*3);
							
						}
					}
				}
				av_free_packet( &packet );
			}
				
		}else{
			av_free_packet( &packet );
		}
	
		
	}
	
	printf("Null frames?\n");
	/*Feed null frames need to get a delayed frame*/
	if (moreFrames){
		printf("Going for a delayed frame %d\n",lastFrame+1);
		fflush(stdout);
		while (1){
			packet.data = NULL;
			packet.size = 0;
			if (avcodec_decode_video2(pCodecCtx, tmp_picture, &frameFinished, 
					&packet) >=0){
			
				if (frameFinished){
					if (packet.pts == frameIndices.at(frameNo).pkt_pts){
						if(img_convert_ctx == NULL){
							if (tmp_picture->linesize[0] != width){ //Hack for padding
								for (int zzz = 0; zzz < height;zzz++){
									memcpy(decodedFrame+zzz*width*3,tmp_picture->data[0]+zzz*tmp_picture->linesize[0]*3,width*sizeof(unsigned char)*3);
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
									memcpy(decodedFrame+zzz*width*3,picture->data[0]+zzz*tmp_picture->linesize[0]*3,width*sizeof(unsigned char)*3);
								}
							} else {
								memcpy(decodedFrame,picture->data[0],width*height*sizeof(unsigned char)*3);
								
							}
						}
						printf("Got a delayed Frame\n");
						fflush(stdout);
						break;
					}

				}else{
					printf("Didn't get a frame anymore %d\n",lastFrame+1);
					fflush(stdout);
					break;				
				}
				
			}else{
				break;
			}
		}
	}
	
	lastFrame = frameNo;
	return frameNo;
}	

VideoReader::~VideoReader(){
	// Close the video file
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
	pFormatCtx = NULL;
	pCodecCtx =NULL;
	//Attempt to free pictures
	avcodec_free_frame(&tmp_picture);
	av_freep(&picture->data[0]);
	avcodec_free_frame(&picture);

	if (&frameIndices != NULL){
		frameIndices.clear();
	}
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

int VideoReader::getNumberOfIndices(){
	return frameIndices.size();
}
