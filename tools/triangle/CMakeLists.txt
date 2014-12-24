# Find triangle
INCLUDE_DIRECTORIES(${Triangle_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
SET(SRC_FILES TriangleWrapper.cpp)
SET(INC_FILES TriangleWrapper.h)

ADD_LIBRARY(lib_triangle_wrapper SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_triangle_wrapper PROPERTIES OUTPUT_NAME "triangle_wrapper")
TARGET_LINK_LIBRARIES(lib_triangle_wrapper Mesh lib_MeshUtils ${Triangle_LIBRARIES})

IF(NOT WITHOUT_SWIG)
    SET(SWG_FILES PyTriangle.i)

    # Generate wrapper for python
    SET_SOURCE_FILES_PROPERTIES(${SWG_FILES} PROPERTIES CPLUSPLUS ON)
    SET_SOURCE_FILES_PROPERTIES(${SWG_FILES} PROPERTIES CMAKE_SWIG_FLAGS "-includeall")
    SWIG_ADD_MODULE(PyTriangle python ${SWG_FILES} ${INC_FILES})
    SWIG_LINK_LIBRARIES(PyTriangle ${PYTHON_LIBRARIES} Mesh lib_triangle_wrapper)

    ADD_CUSTOM_TARGET(triangle DEPENDS lib_triangle_wrapper _PyTriangle)
ELSE(NOT WITHOUT_SWIG)
    ADD_CUSTOM_TARGET(triangle DEPENDS lib_triangle_wrapper)
ENDIF(NOT WITHOUT_SWIG)
