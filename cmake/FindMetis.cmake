# Find METIS library (http://glaros.dtc.umn.edu/gkhome/METIS/METIS/overview)
# The following varaibles are set
#
# METIS_FOUND
# METIS_INCLUDE_DIRS
# METIS_LIBRARIES
#
# It searches the following environment varaibles
# $METIS_PATH, $METIS_ROOT, $METIS_INC, $METIS_LIB

FIND_PATH(METIS_INCLUDE_DIR metis.h
    $ENV{METIS_PATH}
    $ENV{METIS_PATH}/include
    $ENV{METIS_ROOT}
    $ENV{METIS_ROOT}/include
    $ENV{METIS_INC}
    /opt/local/include
    /usr/local/include
    /usr/include/)

FIND_LIBRARY(METIS_LIBRARY metis
    $ENV{METIS_PATH}
    $ENV{METIS_PATH}/lib
    $ENV{METIS_ROOT}
    $ENV{METIS_ROOT}/lib
    $ENV{METIS_LIB}
    /opt/local/lib
    /usr/local/lib
    /usr/lib)

SET(METIS_INCLUDE_DIRS ${METIS_INCLUDE_DIR})
SET(METIS_LIBRARIES ${METIS_LIBRARY})

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(METIS
    "METIS library cannot be found.  Consider set METIS_PATH environment variable"
    METIS_INCLUDE_DIRS METIS_LIBRARIES)

MARK_AS_ADVANCED(
    METIS_INCLUDE_DIRS
    METIS_LIBRARIES)

