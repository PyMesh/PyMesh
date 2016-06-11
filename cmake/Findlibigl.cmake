# Find libigl (http://igl.ethz.ch/projects/libigl/)
# The following variables are set
#
# LIBIGL_FOUND
# LIBIGL_INCLUDE_DIRS
#
# It searches the environment variable $LIBIGL_PATH

FIND_PATH(LIBIGL_INCLUDE_DIRS igl/igl_inline.h
    $ENV{LIBIGL_PATH}
    $ENV{LIBIGL_PATH}/include/
    ${PROJECT_SOURCE_DIR}/third_party/libigl/include
    /opt/local/include/
    /usr/local/include/
    /usr/include/)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBIGL
    "LIBIGL library cannot be found.  Consider set LIBIGL_PATH environment variable"
    LIBIGL_INCLUDE_DIRS)

MARK_AS_ADVANCED(LIBIGL_INCLUDE_DIRS)
