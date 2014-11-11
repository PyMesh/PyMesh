# Find Metis library (http://glaros.dtc.umn.edu/gkhome/metis/metis/overview)
# The following varaibles are set
#
# Metis_FOUND
# Metis_INCLUDE_DIR
# Metis_LIBRARY
#
# It searches the following environment varaibles
# $METIS_PATH, $METIS_ROOT, $METIS_INC, $METIS_LIB

FIND_PATH(Metis_INCLUDE_DIR metis.h
    $ENV{METIS_PATH}
    $ENV{METIS_PATH}/include
    $ENV{METIS_ROOT}
    $ENV{METIS_ROOT}/include
    $ENV{METIS_INC}
    /opt/local/include
    /usr/local/include
    /usr/include/)

FIND_LIBRARY(Metis_LIBRARY metis
    $ENV{METIS_PATH}
    $ENV{METIS_PATH}/lib
    $ENV{METIS_ROOT}
    $ENV{METIS_ROOT}/lib
    $ENV{METIS_LIB}
    /opt/local/lib
    /usr/local/lib
    /usr/lib)


include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Metis
    "Metis library cannot be found.  Consider set METIS_PATH environment variable"
    Metis_INCLUDE_DIR
    Metis_LIBRARY)

MARK_AS_ADVANCED(
    Metis_INCLUDE_DIR
    Metis_LIBRARY)
