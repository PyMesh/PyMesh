## =============================================================================
##  This file is part of the mmg software package for the tetrahedral
##  mesh modification.
##  Copyright (c) Inria - IMB (Université de Bordeaux) - LJLL (UPMC), 2004- .
##
##  mmg is free software: you can redistribute it and/or modify it
##  under the terms of the GNU Lesser General Public License as published
##  by the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  mmg is distributed in the hope that it will be useful, but WITHOUT
##  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
##  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
##  License for more details.
##
##  You should have received a copy of the GNU Lesser General Public
##  License and of the GNU General Public License along with mmg (in
##  files COPYING.LESSER and COPYING). If not, see
##  <http://www.gnu.org/licenses/>. Please read their terms carefully and
##  use this copy of the mmg distribution only if you accept them.
## =============================================================================
#
# This package define the MMG_INCLUDE_DIR and the MMG_LIBRARY variables.  To
# link with the mmg library using CMake add the following lines to your
# CMakeLists.txt:
#
# INCLUDE(FindMmg.cmake)
#
# INCLUDE_DIRECTORIES(${MMG_INCLUDE_DIR})
#
# TARGET_LINK_LIBRARIES( ${YOUR_TARGET} ${MMG_LIBRARY})


IF ((NOT WIN32) AND (NOT WIN64))
  SET ( MMG_INCLUDE_DIR MMG_INCLUDE_DIR-NOTFOUND )
  SET ( MMG_LIBRARY MMG_LIBRARY-NOTFOUND )
ENDIF()

FIND_PATH(MMG_INCLUDE_DIR
  NAMES mmg/libmmg.h
  HINTS ${MMG_INCLUDE_DIR}
  $ENV{MMG_INCLUDE_DIR}
  $ENV{HOME}/include/
  ${MMG_BUILD_DIR}/include/
  $ENV{MMG_BUILD_DIR}/include/
  PATH_SUFFIXES
  DOC "Directory of mmg Headers")

# Check for mmg library
FIND_LIBRARY(MMG_LIBRARY
  NAMES mmg mmg${MMG_LIB_SUFFIX}
  HINTS ${MMG_LIBRARY}
  $ENV{MMG_LIBRARY}
  $ENV{HOME}/lib
  ${MMG_BUILD_DIR}/lib
  $ENV{MMG_BUILD_DIR}/lib
  DOC "The mmg library"
  )

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Mmg DEFAULT_MSG
  MMG_INCLUDE_DIR MMG_LIBRARY)

IF ( NOT Mmg_FOUND )
   MESSAGE (WARNING "\nMMG not found: you can set the 'MMG_BUILD_DIR' environment"
   " variable or the 'MMG_BUILD_DIR' CMake variable to your MMG directory path"
   " to help us to fill the MMG library paths.\n" )
ENDIF ()

