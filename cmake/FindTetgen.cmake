# Find tetgen library (http://wias-berlin.de/software/tetgen/)
# The following variables are set
#
# TETGEN_FOUND
# TETGEN_INCLUDE_DIR
# TETGEN_LIBRARIES
#
# It searches the environment variable $TETGEN_PATH automatically.

FIND_PATH(TETGEN_INCLUDE_DIR tetgen.h
    $ENV{TETGEN_PATH}
    /opt/local/include/
    /usr/local/include/
    /usr/include/)

FIND_LIBRARY(TETGEN_LIBRARIES tet
    $ENV{TETGEN_PATH}
    /opt/local/lib/
    /usr/local/lib/
    /usr/lib/)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Tetgen
    "Tetgen library cannot be found.  Consider set TETGEN_PATH environment variable"
    TETGEN_INCLUDE_DIR
    TETGEN_LIBRARIES)

MARK_AS_ADVANCED(
    TETGEN_INCLUDE_DIR
    TETGEN_LIBRARIES)
