#project(Test)
cmake_minimum_required(VERSION 2.8)

message(STATUS "Current Dir: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "build type is:" ${CMAKE_BUILD_TYPE})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

#pre define
	add_definitions(-DSTREAM_EXPORTS)
#set 
#set()

message("cmake")

FILE(GLOB_RECURSE SRC_LIST "*.c" "*.cpp" "*.h" "*.hpp")

#set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/Code/Include)

#include dir
include_directories(${DIR_INCLUDE}/
					${DIR_INCLUDE}/streaming/
					${DIR_INCLUDE}/web/
					${DIR_3RD_PARTY}/
					${DIR_3RD_PARTY}/ffmpeg
					${DIR_3RD_PARTY}/websocketpp
					${DIR_3RD_PARTY}/sdl/
					${DIR_3RD_PARTY}/sdl1.2/
					${DIR_3RD_PARTY}/rtmp/
	
)


if(WIN32)
set (STREAM_LIB
		debug       SDL2
		debug		avformat
		debug		avcodec
		debug		avutil
		debug		swscale
		debug		librtmp

		optimized	SDL2
		optimized	avformat
		optimized	avcodec
		optimized	avutil
		optimized	swscale
		optimized	librtmp
	)
elseif()
set (STREAM_LIB
		
	)
endif()

#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#link_directories(${PROJECT_SOURCE_DIR}/Lib_d
#				 ${PROJECT_SOURCE_DIR}/Lib_d/ffmpeg	
#				)
#else()
#link_directories(${PROJECT_SOURCE_DIR}/Lib
#				 ${PROJECT_SOURCE_DIR}/Lib/ffmpeg	
#				)
#endif()	


set (PROJECT Streaming)	
		
#add_executable(${PROJECT} ${SRC_LIST})

#编译
add_library(${PROJECT} SHARED ${SRC_LIST})


#设置debug版本输出文件名后缀
set_target_properties(${PROJECT}  PROPERTIES DEBUG_POSTFIX "d")

#link library
target_link_libraries(${PROJECT} ${STREAM_LIB})




