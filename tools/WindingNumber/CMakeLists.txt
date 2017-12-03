# Include this directory
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

SET(DEPENDENT_LIBS "")

# IGL and CGAL
IF (LIBIGL_FOUND AND CGAL_FOUND)
    INCLUDE_DIRECTORIES(${LIBIGL_INCLUDE_DIRS})
    ADD_DEFINITIONS(-DWITH_IGL)
    LIST(APPEND DEPENDENT_LIBS cgal_wrapper)
    ADD_SUBDIRECTORY(IGL)
ELSEIF(NOT LIBIGL_FOUND)
    MESSAGE(WARNING "libigl is missing, libigl winding number support is skipped")
ELSEIF(NOT CGAL_FOUND)
    MESSAGE(WARNING "CGAL is missing, libigl winding number support is skipped")
ENDIF (LIBIGL_FOUND AND CGAL_FOUND)

ADD_LIBRARY(lib_winding_number SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_winding_number PROPERTIES OUTPUT_NAME "winding_number")
TARGET_LINK_LIBRARIES(lib_winding_number Mesh ${DEPENDENT_LIBS})

ADD_CUSTOM_TARGET(winding_number DEPENDS lib_winding_number)
