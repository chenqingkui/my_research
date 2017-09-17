cmake_minimum_required(VERSION 2.6)
FILE(GLOB_RECURSE SRC_LIST "*.cpp" "*.h" )


if(NOT WIN32)
	SET( VtThriftInclude "thrift-linux")
	#SET( VtThriftInclude "thrift")
ELSE()
	SET( VtThriftInclude "thrift-win")
endif()

# 头文件.h的路径
include_directories(
	${DIR_INCLUDE}/${VtThriftInclude}
	${DIR_INCLUDE}
	${DIR_INCLUDE}/IVE
	${DIR_INCLUDE}/SCFL/os
	${DIR_INCLUDE}/SCFL/file
	${DIR_INCLUDE}/SCFL/time
	${DIR_INCLUDE}/SCFL/thread
	${DIR_INCLUDE}/json
	${DIR_INCLUDE}/streaming/
	)

message(STATUS "Current Dir: " ${CMAKE_CURRENT_SOURCE_DIR})


#平台无关库
SET(COMMON_LIBS 
	)
	
#平台特定库
IF(WIN32)	
	SET(PLATFORM_LIBS
	debug thriftmdd
	debug streamingd
	optimized thriftmd
	optimized streamingd
	#optimized libssl
	#optimized libcrypto
	)
ELSEIF(UNIX)
	SET(PLATFORM_LIBS  thrift-0.9.3 ssl crypto json xml2 asound ini z iconv pthread)
ENDIF()


SET(PROJECT ThriftServerDemo)

#编译
add_executable(${PROJECT} ${SRC_LIST} )

#Debug版本后缀
set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX "d")

#连接库
target_link_libraries(${PROJECT}  ${COMMON_LIBS} ${PLATFORM_LIBS})
