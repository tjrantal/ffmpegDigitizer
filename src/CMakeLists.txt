# Properties->C/C++->General->Additional Include Directories
#include_directories ("${PROJECT_SOURCE_DIR}/lib")
#include_directories ("D:/timo/programming/cpp/ocvBuildVCC/install/include")
#LINK_DIRECTORIES("D:/timo/programming/cpp/ocvBuildVCC/lib/Release")

file (GLOB SOURCE_FILES "*.h" "*.cpp")

# Collect sources into the variable SIMPLE_FUNCTION_SOURCES  
file (GLOB digitizerFrame_FUNCTION_SOURCES
      "digitizerFrame/*.h"
      "digitizerFrame/*.cpp")

file (GLOB imagePanel_FUNCTION_SOURCES
      "imagePanel/*.h"
      "imagePanel/*.cpp")
      
file (GLOB markerSelector_FUNCTION_SOURCES
      "markerSelector/*.h"
      "markerSelector/*.cpp")
      
file (GLOB trackingThread_FUNCTION_SOURCES
      "trackingThread/*.h"
      "trackingThread/*.cpp")

file (GLOB videoReader_FUNCTION_SOURCES
      "videoReader/*.h"
      "videoReader/*.cpp")
	  
source_group("" FILES ${SOURCE_FILES})
source_group("digitizerFrame" FILES ${digitizerFrame_FUNCTION_SOURCES})
source_group("imagePanel" FILES ${imagePanel_FUNCTION_SOURCES})
source_group("markerSelector" FILES ${markerSelector_FUNCTION_SOURCES})
source_group("trackingThread" FILES ${trackingThread_FUNCTION_SOURCES})
source_group("videoReader" FILES ${videoReader_FUNCTION_SOURCES})


#ADD dependent libraries include and link directories
INCLUDE_DIRECTORIES( ${wxWidgets_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( ${FFMPEG_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( ${OpenCV_DIR}/install/include )
#INCLUDE_DIRECTORIES( ${GSL_INCLUDE_DIRS} )
#link_directories(${OpenCV_LIB_DIR})

# Set Properties->General->Configuration Type to Application(.exe)
# Creates digitizer.exe with the listed sources (main.cxx)
# Adds sources to the Solution Explorer
#add_executable (digitizer digitizer.cpp utils/CSVReader.cpp dlt/)


if (WIN32)
	message("Executable WIN32")
	add_executable (digitizer ${SOURCE_FILES}  ${digitizerFrame_FUNCTION_SOURCES} ${imagePanel_FUNCTION_SOURCES} ${markerSelector_FUNCTION_SOURCES} ${trackingThread_FUNCTION_SOURCES} ${videoReader_FUNCTION_SOURCES})
endif(WIN32)
if(UNIX)
	add_executable (digitizer ${SOURCE_FILES}  ${digitizerFrame_FUNCTION_SOURCES} ${imagePanel_FUNCTION_SOURCES} ${markerSelector_FUNCTION_SOURCES} ${trackingThread_FUNCTION_SOURCES} ${videoReader_FUNCTION_SOURCES})
endif(UNIX)

#message("FFMPEG_LIBRARIES  " ${FFMPEG_LIBRARIES})
#message("wxWidgets_LIBRARIES  " ${wxWidgets_LIBRARIES})

#Get the missing dependent libs for ffmpeg...
#find_package(PkgConfig REQUIRED)
#pkg_search_module(LIBAVFORMAT REQUIRED libavformat)

#message("LIBAVFORMAT " ${LIBAVFORMAT_LIBRARIES})

# Properties->Linker->Input->Additional Dependencies
if(UNIX)
	target_link_libraries (digitizer ${FFMPEG_LIBRARIES} ${wxWidgets_LIBRARIES} ${OpenCV_LIBS} -lX11)#${LIBAVFORMAT_LIBRARIES})# ${GSL_LIBRARIES})
endif(UNIX)
if(WIN32)
	target_link_libraries (digitizer ${FFMPEG_LIBRARIES} ${wxWidgets_LIBRARIES} ${OpenCV_LIBS})
endif(WIN32)
#message("wxWidgets wxWidgets_LIBRARIES ${wxWidgets_LIBRARIES}")
 
# Creates a folder "executables" and adds target 
# project (digitizer.vcproj) under it
set_property(TARGET digitizer PROPERTY FOLDER "executables")
 
# Adds logic to INSTALL.vcproj to copy digitizer.exe to destination directory
install (TARGETS digitizer
         RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)
