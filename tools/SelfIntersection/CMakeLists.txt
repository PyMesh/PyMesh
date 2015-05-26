# Include this directory
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

SET(DEPENDENT_LIBS "")

# IGL and CGAL
IF (LIBIGL_FOUND AND CGAL_FOUND)
    INCLUDE_DIRECTORIES(${LIBIGL_INCLUDE_DIRS})
    INCLUDE(${CGAL_USE_FILE})
    ADD_DEFINITIONS(-DWITH_IGL)
    LIST(APPEND DEPENDENT_LIBS ${CGAL_LIBRARIES})
    ADD_SUBDIRECTORY(IGL)
ELSEIF(NOT LIBIGL_FOUND)
    MESSAGE(WARNING "libigl is missing, libigl self-intersection support is skipped")
ELSEIF(NOT CGAL_FOUND)
    MESSAGE(WARNING "CGAL is missing, libigl self-intersection support is skipped")
ENDIF (LIBIGL_FOUND AND CGAL_FOUND)

ADD_LIBRARY(lib_self_intersection SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_self_intersection PROPERTIES OUTPUT_NAME "self_intersection")
TARGET_LINK_LIBRARIES(lib_self_intersection Mesh ${DEPENDENT_LIBS})

IF(NOT WITHOUT_SWIG)
    set(SWG_FILES PySelfIntersection.i)

    # Generate wrapper for python
    SET_SOURCE_FILES_PROPERTIES(${SWG_FILES} PROPERTIES CPLUSPLUS ON)
    SET_SOURCE_FILES_PROPERTIES(${SWG_FILES} PROPERTIES CMAKE_SWIG_FLAGS "-includeall")
    SWIG_ADD_MODULE(PySelfIntersection python ${SWG_FILES} ${INC_FILES})
    SWIG_LINK_LIBRARIES(PySelfIntersection ${PYTHON_LIBRARIES} Mesh
        lib_self_intersection ${DEPENDENT_LIBS})

    # Boudle lib, swig wrapper into a single target.
    # Usage: make self_intersection
    ADD_CUSTOM_TARGET(self_intersection DEPENDS lib_self_intersection
        _PySelfIntersection)
ELSE(NOT WITHOUT_SWIG)
    ADD_CUSTOM_TARGET(self_intersection DEPENDS lib_self_intersection)
ENDIF(NOT WITHOUT_SWIG)
