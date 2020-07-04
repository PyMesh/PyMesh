# Find TetWild
# This module searches for TetWild library (https://github.com/Yixin-Hu/TetWild)
#
# The module defines the following variables:
#   TETWILD_FOUND:        Defined only if TetWild is found.
#   TETWILD_INCLUDE_DIRS: The include directory containing `tetwild.h`
#   TETWILD_LIBRARY:      The full path of TetWild library.
#
# It searches the environment variable $TETWILD_PATH automatically.

FIND_PATH(TETWILD_INCLUDE_DIRS tetwild.h
    PATHS
    $ENV{TETWILD_PATH}
    $ENV{TETWILD_PATH}/include
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/include/
    PATH_SUFFIXES
    tetwild
    NO_DEFAULT_PATH)

FIND_LIBRARY(TETWILD_LIBRARY TetWild
    PATHS
    $ENV{TETWILD_PATH}
    $ENV{TETWILD_PATH}/lib/
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/lib/
    PATH_SUFFIXES
    tetwild
    NO_DEFAULT_PATH)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TetWild
    "TetWild library cannot be found.  Consider set TETWILD_PATH environment variable"
    TETWILD_INCLUDE_DIRS
    TETWILD_LIBRARY)

MARK_AS_ADVANCED(
    TETWILD_INCLUDE_DIRS
    TETWILD_LIBRARY)

