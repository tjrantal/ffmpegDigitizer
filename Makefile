CC=g++
#CFLAGS=-c -Wall
#Try to figure out if compiling on mingw windows or on linux
SYS := $(shell gcc -dumpmachine)
ifneq (, $(findstring linux, $(SYS)))
	# Do linux things
   CFLAGS=-c -Wall $(shell wx-config --cxxflags) $(shell pkg-config --cflags libavformat libavcodec libswscale x264)
   LDFLAGS= -O2 -march=i686 $(shell wx-config --libs) $(shell pkg-config --libs libavformat libavcodec libswscale x264)
else ifneq (, $(findstring mingw, $(SYS)))
   CFLAGS=-c -Wall $(shell wx-config --cxxflags) $(shell pkg-config --cflags libavformat libavcodec libswscale x264)
   LDFLAGS= -static -static-libgcc -O2 -march=i686 $(shell wx-config --libs) $(shell pkg-config --libs libavformat libavcodec libswscale x264) -lpthreadGC2			
   # Do mingw things
else ifneq (, $(findstring cygwin, $(SYS)))
   # Do cygwin things
else
   # Do things for others
endif

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
