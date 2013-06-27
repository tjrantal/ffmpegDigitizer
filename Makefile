CC=g++
#CFLAGS=-c -Wall
CFLAGS=-c -Wall $(shell wx-config --cxxflags) $(shell pkg-config --cflags libavformat libavcodec libswscale x264)
LDFLAGS= -static -O3 -march=i686 $(shell wx-config --libs) $(shell pkg-config --libs libavformat libavcodec libswscale x264) -lpthreadGC2
#LDFLAGS= -O3 -march=i686 $(shell wx-config --libs) $(shell pkg-config --libs libavformat libavcodec libswscale x264)
#pkg-config --libs libavformat libavcodec libswscale

#-lgsl -lgslcblas
SOURCES=imagePanel/ImagePanel.cpp videoReader/VideoReader.cpp digitizerFrame/DigitizerFrame.cpp Digitizer.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=digitizer.exe
#	@echo ${CC} ${LDFLAGS} ${OBJECTS} -o ${@}
#	@echo ${CC} ${CFLAGS} ${<} -o ${@}
#	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

.cpp.o:
	@echo ${<} ${@}
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm *.o imagePanel/*.o videoReader/*.o digitizerFrame/*.o $(EXECUTABLE)
