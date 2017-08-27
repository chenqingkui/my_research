cmake_minimum_required(VERSION 2.8)

project(MyTest)

message(STATUS "cur path" ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE  my_project_files  "*.cpp" "*.h " "*.hpp")


set( DIR_INCLUDE  ${CMAKE_CURRENT_SOURCE_DIR}/include)
set( ALG_INCLUDE  ${CMAKE_CURRENT_SOURCE_DIR}/../../alg)
include_directories(${DIR_INCLUDE}
                    ${ALG_INCLUDE}/sort/)

add_definitions(-DWIN32)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib_d)
else()
	link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)
endif()


set(my_libs  )

#add_library()

set(project MyAlgTest)

add_executable(${project} ${my_project_files} )

set_target_properties(${project} PROPERTIES DEBUG_POSTFIX "d")

#set_target_properties(${project} ${my_libs})

target_link_libraries(${project} ${my_libs})