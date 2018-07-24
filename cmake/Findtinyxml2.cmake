# Find TINYXML2 library (https://github.com/gilbo/TINYXML2)
# The following variables are set
#
# TINYXML2_FOUND
# TINYXML2_INCLUDE_DIRS
# TINYXML2_LIBRARIES
#
# It searches the environment variable $TINYXML2_PATH automatically.

FIND_PATH(TINYXML2_INCLUDE_DIRS tinyxml2.h
    $ENV{TINYXML2_PATH}
    $ENV{TINYXML2_PATH}/include/
    /opt/local/include/
    /usr/local/include/
    /usr/include/)

FIND_LIBRARY(TINYXML2_LIBRARIES tinyxml2
    $ENV{TINYXML2_PATH}
    $ENV{TINYXML2_PATH}/lib/
    /opt/local/lib/
    /usr/local/lib/
    /usr/lib/)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(tinyxml2
    "tinyxml2 library cannot be found.  Consider set TINYXML2_PATH environment variable"
    TINYXML2_INCLUDE_DIRS
    TINYXML2_LIBRARIES)

MARK_AS_ADVANCED(
    TINYXML2_INCLUDE_DIRS
    TINYXML2_LIBRARIES)

IF (TINYXML2_FOUND AND NOT TARGET PyMesh::TinyXML2)
    ADD_LIBRARY(PyMesh::TinyXML2 INTERFACE IMPORTED)
    TARGET_INCLUDE_DIRECTORIES(PyMesh::TinyXML2 SYSTEM
        INTERFACE
            ${TINYXML2_INCLUDE_DIRS}
    )
    TARGET_LINK_LIBRARIES(PyMesh::TinyXML2
        INTERFACE
            ${TINYXML2_LIBRARIES}
    )
ENDIF (TINYXML2_FOUND AND NOT TARGET PyMesh::TinyXML2)
