# Find Cork library (https://github.com/gilbo/cork)
# The following variables are set
#
# CORK_FOUND
# CORK_INCLUDE_DIRS
# CORK_LIBRARIES
#
# It searches the environment variable $CORK_PATH automatically.

FIND_PATH(CORK_INCLUDE_DIRS cork.h
    $ENV{CORK_PATH}
    $ENV{CORK_PATH}/include/
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/include/
    /opt/local/include/
    /usr/local/include/
    /usr/include/)

FIND_LIBRARY(CORK_LIBRARIES cork
    $ENV{CORK_PATH}
    $ENV{CORK_PATH}/lib/
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/lib/
    /opt/local/lib/
    /usr/local/lib/
    /usr/lib/)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Cork
    "Cork library cannot be found.  Consider set CORK_PATH environment variable"
    CORK_INCLUDE_DIRS
    CORK_LIBRARIES)

MARK_AS_ADVANCED(
    CORK_INCLUDE_DIRS
    CORK_LIBRARIES)
