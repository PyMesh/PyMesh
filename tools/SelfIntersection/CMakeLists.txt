# Include this directory
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

SET(DEPENDENT_LIBS ${Boost_LIBRARIES})
SET(DEPENDENT_INCLUDES "")

# IGL and CGAL
IF (LIBIGL_FOUND AND CGAL_FOUND)
    LIST(APPEND DEPENDENT_INCLUDES ${LIBIGL_INCLUDE_DIRS})
    ADD_DEFINITIONS(-DWITH_IGL)
    LIST(APPEND DEPENDENT_LIBS cgal_wrapper)
    ADD_SUBDIRECTORY(IGL)
ELSEIF(NOT LIBIGL_FOUND)
    MESSAGE(WARNING "libigl is missing, libigl self-intersection support is skipped")
ELSEIF(NOT CGAL_FOUND)
    MESSAGE(WARNING "CGAL is missing, libigl self-intersection support is skipped")
ENDIF (LIBIGL_FOUND AND CGAL_FOUND)

ADD_LIBRARY(lib_self_intersection SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_self_intersection PROPERTIES OUTPUT_NAME "self_intersection")
TARGET_INCLUDE_DIRECTORIES(lib_self_intersection SYSTEM PRIVATE
    ${DEPENDENT_INCLUDES})
TARGET_LINK_LIBRARIES(lib_self_intersection Mesh ${DEPENDENT_LIBS})

ADD_CUSTOM_TARGET(self_intersection DEPENDS lib_self_intersection)
