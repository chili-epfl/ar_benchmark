# - Try to find the artoolkitplus library
# Once done this will define
#
#  ARTOOLKITPLUS_FOUND - system has ARTOOLKITPLUS
#  ARTOOLKITPLUS_INCLUDE_DIR - the ARTOOLKITPLUS include directory
#  ARTOOLKITPLUS_LIBRARIES - Link these to use ARTOOLKITPLUS
#  ARTOOLKITPLUS_LINK_DIRECTORIES - link directories, useful for rpath
#

if (ARTOOLKITPLUS_INCLUDE_DIR AND ARTOOLKITPLUS_LIBRARIES)

  # in cache already
  SET(ARTOOLKITPLUS_FOUND TRUE)

else (ARTOOLKITPLUS_INCLUDE_DIR AND ARTOOLKITPLUS_LIBRARIES)

  if (WIN32)
    
    # check whether the ARTOOLKITPLUSDIR environment variable is set and points to a 
    # valid windows ARTOOLKITPLUS installation
    FIND_PATH(
	ARTOOLKITPLUS_DIR include/ARToolKitPlus/ARToolKitPlus.h
	PATHS $ENV{ARTOOLKITPLUSROOT}/ "C:/Program Files/ARToolKitPlus/"
      DOC "The main directory of the ARTOOLKITPLUS library, containing the subfolders include and lib" )
    
    if (ARTOOLKITPLUS_DIR)
      SET(ARTOOLKITPLUS_INCLUDE_DIR ${ARTOOLKITPLUS_DIR}/include CACHE PATH "ARTOOLKITPLUS include directory")
      SET(ARTOOLKITPLUS_LINK_DIRECTORIES ${ARTOOLKITPLUS_DIR}/lib CACHE PATH "ARTOOLKITPLUS link directory")
      SET(ARTOOLKITPLUS_LIBRARIES ARTOOLKITPLUS CACHE STRING "ARTOOLKITPLUS library name")
      SET(ARTOOLKITPLUS_FOUND TRUE)
    else (ARTOOLKITPLUS_DIR)
      SET (ARTOOLKITPLUS_FOUND FALSE)
    endif (ARTOOLKITPLUS_DIR)

  else (WIN32)

    FIND_PATH(ARTOOLKITPLUS_INCLUDE_DIR ARToolKitPlus/ARToolKitPlus.h
      /usr/include)

    FIND_LIBRARY(ARTOOLKITPLUS_LIBRARIES NAMES ARToolKitPlus PATHS
      /usr/lib
      /usr/lib64)
    
    if(ARTOOLKITPLUS_INCLUDE_DIR AND ARTOOLKITPLUS_LIBRARIES)
      set(ARTOOLKITPLUS_FOUND TRUE)
    endif(ARTOOLKITPLUS_INCLUDE_DIR AND ARTOOLKITPLUS_LIBRARIES)

  endif (WIN32)
  
  if (ARTOOLKITPLUS_FOUND)
    if (NOT ARToolKitPlus_FIND_QUIETLY)
      message(STATUS "Found ARTOOLKITPLUS: ${ARTOOLKITPLUS_LIBRARIES}")
    endif (NOT ARToolKitPlus_FIND_QUIETLY)
  else (ARTOOLKITPLUS_FOUND)
    if (ARToolKitPlus_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find ARTOOLKITPLUS")
    endif (ARToolKitPlus_FIND_REQUIRED)
  endif (ARTOOLKITPLUS_FOUND)

  
endif (ARTOOLKITPLUS_INCLUDE_DIR AND ARTOOLKITPLUS_LIBRARIES)
