# Find Carve library
# (https://code.google.com/p/carve/)
# The following variables are set
#
# CARVE_FOUND
# CARVE_INCLUDE_DIRS
# CARVE_LIBRARIES
#
# It searches the environment variable $CARVE_PATH automatically.

FIND_PATH(CARVE_INCLUDE_DIRS
    name carve/carve.hpp
    PATHS
    $ENV{CARVE_PATH}
    $ENV{CARVE_PATH}/include/
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/include/
    /opt/local/include/
    /usr/local/include/
    /usr/include/)

FIND_LIBRARY(CARVE_LIBRARIES carve
    $ENV{CARVE_PATH}
    $ENV{CARVE_PATH}/lib/
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/lib/
    /opt/local/lib/
    /usr/local/lib/
    /usr/lib/)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CARVE
    "Carve library cannot be found.  Consider set CARVE_PATH environment variable"
    CARVE_INCLUDE_DIRS
    CARVE_LIBRARIES)

MARK_AS_ADVANCED(
    CARVE_INCLUDE_DIRS
    CARVE_LIBRARIES)
