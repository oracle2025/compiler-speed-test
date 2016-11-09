project(compiler-speed-test)
cmake_minimum_required(VERSION 2.8)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

set(Boost_USE_MULTITHREADED ON)

find_package(Boost 1.46  REQUIRED COMPONENTS filesystem system date_time thread program_options)

set(OpenCV_DIR "${PROJECT_SOURCE_DIR}/../aeoon-sambaliblog/libs/opencv")
find_package( OpenCV REQUIRED )

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "(GNU|Clang)")
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wno-deprecated")
endif()


add_executable(main main.cpp)
add_executable(iomain iomain.cpp)

add_executable(boostmain boostmain.cpp)
target_include_directories(boostmain PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(boostmain ${Boost_LIBRARIES}
    wsock32 ws2_32)

add_executable(opencvmain opencvmain.cpp)
target_include_directories(opencvmain PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(opencvmain ${OpenCV_LIBRARIES})

if(WIN32)
	add_definitions(-Dclew_STATIC)
endif(WIN32)

add_executable(computemain
	computemain.cpp
	get_file_contents.h
	get_file_contents.cpp
	shadertest.h
	shadertest.cpp
	#shadertestimpl.h
	#shadertestimpl.cpp
	last_modified.h
	last_modified.cpp)
target_include_directories(computemain PRIVATE ${Boost_INCLUDE_DIR}
	"${PROJECT_SOURCE_DIR}/clew/include"
	"${PROJECT_SOURCE_DIR}/clew/include/proxy-opencl"
	)
target_link_libraries(computemain clew ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})

add_subdirectory(clew)
