# Find GeoGram library
# (http://alice.loria.fr/software/geogram/doc/html/index.html)
# The following variables are set
#
# GEOGRAM_FOUND
# GEOGRAM_INCLUDE_DIRS
# GEOGRAM_LIBRARIES
#
# It searches the environment variable
# $GEOGRAM_PATH and $GEOGRAM_INSTALL_PREFIX automatically.

FIND_PATH(GEOGRAM_INCLUDE_DIRS
    geogram/basic/common.h
    ${GEOGRAM_INSTALL_PREFIX}
    $ENV{GEOGRAM_INSTALL_PREFIX}
    $ENV{GEOGRAM_PATH}
    /opt/local
    /usr/local
    PATH_SUFFIXES include/geogram1
    )

FIND_LIBRARY(GEOGRAM_LIBRARIES geogram
    ${GEOGRAM_INSTALL_PREFIX}
    $ENV{GEOGRAM_INSTALL_PREFIX}
    $ENV{GEOGRAM_PATH}
    /opt/local
    /usr/local
    PATH_SUFFIXES lib
    )

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GeoGram
    "GeoGram library cannot be found.  Consider set GEOGRAM_PATH environment variable"
    GEOGRAM_INCLUDE_DIRS
    GEOGRAM_LIBRARIES)

MARK_AS_ADVANCED(
    GEOGRAM_INCLUDE_DIRS
    GEOGRAM_LIBRARIES)
