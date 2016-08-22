# Find tetgen library (http://wias-berlin.de/software/tetgen/)
# The following variables are set
#
# TETGEN_FOUND
# TETGEN_INCLUDE_DIRS
# TETGEN_LIBRARIES
#
# It searches the environment variable $TETGEN_PATH automatically.

FIND_PATH(TETGEN_INCLUDE_DIRS tetgen.h
    $ENV{TETGEN_PATH}
    $ENV{TETGEN_PATH}/include/
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/include/
    /opt/local/include/
    /usr/local/include/
    /usr/include/)

FIND_LIBRARY(TETGEN_LIBRARIES NAMES tetgen tet
    PATHS
    $ENV{TETGEN_PATH}
    $ENV{TETGEN_PATH}/lib/
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/lib/
    /opt/local/lib/
    /usr/local/lib/
    /usr/lib/)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Tetgen
    "Tetgen library cannot be found.  Consider set TETGEN_PATH environment variable"
    TETGEN_INCLUDE_DIRS
    TETGEN_LIBRARIES)

MARK_AS_ADVANCED(
    TETGEN_INCLUDE_DIRS
    TETGEN_LIBRARIES)
