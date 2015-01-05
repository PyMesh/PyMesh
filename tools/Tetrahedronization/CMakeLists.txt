# Include this directory
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# Source files
FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

SET(DEPENDENT_LIBS "")

# Inlclude CGAL
IF(CGAL_FOUND)
    SET(BACKUP_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    INCLUDE(${CGAL_USE_FILE})
    SET(CMAKE_CXX_FLAGS ${BACKUP_CXX_FLAGS})
    ADD_DEFINITIONS(-DWITH_CGAL)
    #ADD_DEFINITIONS(-DCGAL_CONCURRENT_MESH_3)
    LIST(APPEND DEPENDENT_LIBS ${CGAL_LIBRARIES})
    ADD_SUBDIRECTORY(CGAL)
ELSE(CGAL_FOUND)
    MESSAGE(WARNING "CGAL missing.  Skipping CGAL tetrahedronization support")
ENDIF(CGAL_FOUND)

ADD_LIBRARY(lib_tetrahedronization SHARED ${SRC_FILES} ${INC_FILES})
TARGET_LINK_LIBRARIES(lib_tetrahedronization
    ${DEPENDENT_LIBS} Mesh lib_MeshUtils)
SET_TARGET_PROPERTIES(lib_tetrahedronization PROPERTIES OUTPUT_NAME
    "tetrahedronization")

IF(NOT WITHOUT_SWIG)
    SET(SWG_FILES PyTetrahedronization.i)

    # Generate wrapper for python
    SET_SOURCE_FILES_PROPERTIES(${SWG_FILES} PROPERTIES CPLUSPLUS ON)
    SET_SOURCE_FILES_PROPERTIES(${SWG_FILES} PROPERTIES CMAKE_SWIG_FLAGS "-includeall")
    SWIG_ADD_MODULE(PyTetrahedronization python ${SWG_FILES} ${INC_FILES})
    SWIG_LINK_LIBRARIES(PyTetrahedronization ${PYTHON_LIBRARIES} lib_tetrahedronization)

    # Boudle exe, lib, swig wrapper into a single target.
    # To build all: "make tetrahedronization"
    ADD_CUSTOM_TARGET(tetrahedronization DEPENDS
        lib_tetrahedronization _PyTetrahedronization)
ELSE(NOT WITHOUT_SWIG)
    ADD_CUSTOM_TARGET(tetrahedronization DEPENDS lib_tetrahedronization)
ENDIF(NOT WITHOUT_SWIG)

