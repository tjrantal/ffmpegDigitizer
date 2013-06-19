CC=g++
#CFLAGS=-c -Wall
CFLAGS=-c -Wall $(shell wx-config --cxxflags) $(shell pkg-config --cflags libavformat) $(shell pkg-config --cflags libavcodec) $(shell pkg-config --cflags libswscale)
LDFLAGS= -static -O3 -march=i686 $(shell wx-config --libs) $(shell pkg-config --libs libavformat) $(shell pkg-config --libs libavcodec) $(shell pkg-config --libs libswscale)
#-lgsl -lgslcblas
SOURCES=Digitizer.cpp
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
	rm *.o $(EXECUTABLE)