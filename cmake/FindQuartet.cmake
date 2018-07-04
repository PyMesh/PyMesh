# Find Quartet library (https://github.com/crawforddoran/quartet)
# The following variables are set
#
# QUARTET_FOUND
# QUARTET_INCLUDE_DIRS
# QUARTET_LIBRARIES
#
# It searches the environment variable $QUARTET_PATH automatically.

FIND_PATH(QUARTET_INCLUDE_DIRS
    NAMES make_tet_mesh.h
    PATHS
    $ENV{QUARTET_PATH}
    $ENV{QUARTET_PATH}/include/
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/include/
    /opt/local/include/
    /usr/local/include/
    /usr/include/
    PATH_SUFFIXES quartet)

FIND_LIBRARY(QUARTET_LIBRARIES quartet
    $ENV{QUARTET_PATH}
    $ENV{QUARTET_PATH}/lib/
    ${PROJECT_SOURCE_DIR}/python/pymesh/third_party/lib/
    /opt/local/lib/
    /usr/local/lib/
    /usr/lib/)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Quartet
    "Quartet library cannot be found.  Consider set QUARTET_PATH environment variable"
    QUARTET_INCLUDE_DIRS
    QUARTET_LIBRARIES)

MARK_AS_ADVANCED(
    QUARTET_INCLUDE_DIRS
    QUARTET_LIBRARIES)

IF (QUARTET_FOUND AND NOT TARGET PyMesh::Quartet)
    ADD_LIBRARY(PyMesh::Quartet INTERFACE IMPORTED)
    TARGET_INCLUDE_DIRECTORIES(PyMesh::Quartet SYSTEM
        INTERFACE
            ${QUARTET_INCLUDE_DIRS}
    )
TARGET_LINK_LIBRARIES(PyMesh::Quartet
        INTERFACE
            ${QUARTET_LIBRARIES}
    )
ENDIF (QUARTET_FOUND AND NOT TARGET PyMesh::Quartet)
