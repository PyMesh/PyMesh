# Find FastWindingNumber library (https://github.com/sideeffects/WindingNumber)
# The following variables are set
#
# FastWindingNumber_FOUND
# FAST_WINDING_NUMBER_INCLUDE_DIRS
# FAST_WINDING_NUMBER_LIBRARIES
#
# It searches the environment variable $FAST_WINDING_NUMBER_PATH automatically.

FIND_PATH(FAST_WINDING_NUMBER_INCLUDE_DIRS UT_SolidAngle.h
    PATHS
    $ENV{FAST_WINDING_NUMBER_PATH}
    $ENV{FAST_WINDING_NUMBER_PATH}/include/
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/include/
    /opt/local/include/
    /usr/local/include/
    /usr/include/
    PATH_SUFFIXES
    FastWindingNumber)

FIND_LIBRARY(FAST_WINDING_NUMBER_LIBRARIES FastWindingNumber
    $ENV{FAST_WINDING_NUMBER_PATH}
    $ENV{FAST_WINDING_NUMBER_PATH}/lib/
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/lib/
    /opt/local/lib/
    /usr/local/lib/
    /usr/lib/)

IF (NOT TBB_FOUND)
    FIND_PACKAGE(TBB)
ENDIF (NOT TBB_FOUND)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FastWindingNumber
    "FastWindingNumber library cannot be found.  Consider set FAST_WINDING_NUMBER_PATH environment variable"
    FAST_WINDING_NUMBER_INCLUDE_DIRS
    FAST_WINDING_NUMBER_LIBRARIES)

MARK_AS_ADVANCED(
    FAST_WINDING_NUMBER_INCLUDE_DIRS
    FAST_WINDING_NUMBER_LIBRARIES)

