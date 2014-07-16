cmake_minimum_required(VERSION 2.8)

project(ar_benchmark)
add_definitions(-std=c++11)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

## System dependencies
find_package(OpenCV REQUIRED)
find_package(PkgConfig)

find_package(ALVAR REQUIRED)
find_package(ARToolKitPlus REQUIRED)
find_package(aruco REQUIRED)
pkg_search_module(CHILITAGS REQUIRED chilitags)

include_directories(include 
                    ${OpenCV_INCLUDE_DIRS} 
                    ${ALVAR_INCLUDE_DIRS} 
                    ${ARTOOLKITPLUS_INCLUDE_DIR} 
                    ${aruco_INCLUDE_DIRS} 
                    ${CHILITAGS_INCLUDE_DIRS})

add_executable(benchmark src/benchmark.cpp)
target_link_libraries(benchmark 
                        ${OpenCV_LIBRARIES} 
                        ${ALVAR_LIBRARIES}
                        ${ARTOOLKITPLUS_LIBRARIES} 
                        ${aruco_LIBS}
                        ${CHILITAGS_LDFLAGS})
install(TARGETS benchmark RUNTIME DESTINATION bin)


