#Makefile obtained from http://mrbook.org/tutorials/make/ and adated to the project's needs
#CC=g++
#CFLAGS=-c -Wall
#Try to figure out if compiling on mingw windows or on linux -Wall
SYS := $(shell gcc -dumpmachine)
ifneq (, $(findstring linux, $(SYS)))
	# Do linux things
	CC=g++
   CFLAGS=-c -w -std=gnu++11 $(shell wx-config --cxxflags) $(shell pkg-config --cflags libavformat libavcodec libswscale x264)
   LDFLAGS= -O2 $(shell wx-config --libs) $(shell pkg-config --libs libavformat libavcodec libswscale x264) -lpthread -std=gnu++11
else ifneq (, $(findstring mingw, $(SYS)))
   # Do mingw things w64
   CC=x86_64-w64-mingw32-g++.exe
   CFLAGS=-c -w -std=gnu++11 $(shell wx-config --cxxflags) $(shell pkg-config --cflags libavformat libavcodec libswscale x264)
   LDFLAGS= -static -static-libgcc -O2 $(shell wx-config --libs) $(shell pkg-config --libs libavformat libavcodec libswscale x264) -lpthread -std=gnu++11
   # Do mingw things
else ifneq (, $(findstring cygwin, $(SYS)))
   # Do cygwin things, haven't tested this copied mingw
   CC=g++
   CFLAGS=-c -w -std=gnu++11 $(shell wx-config --cxxflags) $(shell pkg-config --cflags libavformat libavcodec libswscale x264)
   LDFLAGS= -static -static-libgcc -O2 $(shell wx-config --libs) $(shell pkg-config --libs libavformat libavcodec libswscale x264) -lpthread -std=gnu++11
else
   # Do things for others, haven't tested this, copied linux
   CC=g++
   CFLAGS=-c -w -std=gnu++11 $(shell wx-config --cxxflags) $(shell pkg-config --cflags libavformat libavcodec libswscale x264)
   LDFLAGS= -O2 -march=i686 $(shell wx-config --libs) $(shell pkg-config --libs libavformat libavcodec libswscale x264) -lpthread -std=gnu++11
endif

#-lgsl -lgslcblas
SOURCES=imagePanel/ImagePanel.cpp videoReader/VideoReader.cpp digitizerFrame/DigitizerFrame.cpp markerSelector/MarkerSelector.cpp trackingThread/TrackingThread.cpp Digitizer.cpp 
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
	rm *.o imagePanel/*.o videoReader/*.o digitizerFrame/*.o markerSelector/*.o trackingThread/*.o $(EXECUTABLE)
	
doxygen:
	doxygen
