# Find Jigsaw library (https://github.com/dengwirda/jigsaw)
# The following variables are set
#
# JIGSAW_FOUND
# JIGSAW_INCLUDE_DIRS
# JIGSAW_LIBRARIES
#
# It searches the environment variable $JIGSAW_PATH automatically.

FIND_PATH(JIGSAW_INCLUDE_DIRS lib_jigsaw.h
    $ENV{JIGSAW_PATH}
    $ENV{JIGSAW_PATH}/include/
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/include/
    /opt/local/include/
    /usr/local/include/
    /usr/include/)

FIND_LIBRARY(JIGSAW_LIBRARIES jigsaw
    $ENV{JIGSAW_PATH}
    $ENV{JIGSAW_PATH}/lib/
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/lib/
    /opt/local/lib/
    /usr/local/lib/
    /usr/lib/)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Jigsaw
    "Jigsaw library cannot be found.  Consider set JIGSAW_PATH environment variable"
    JIGSAW_INCLUDE_DIRS
    JIGSAW_LIBRARIES)

MARK_AS_ADVANCED(
    JIGSAW_INCLUDE_DIRS
    JIGSAW_LIBRARIES)

