# Include this directory
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# Google sparse/dense hash
IF (SPARSEHASH_FOUND)
    ADD_DEFINITIONS(-DUSE_SPARSEHASH)
    INCLUDE_DIRECTORIES(${SPARSEHASH_INCLUDE_DIR})
ENDIF (SPARSEHASH_FOUND)

# Enumerate source files
FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(lib_MeshUtils SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_MeshUtils PROPERTIES OUTPUT_NAME "MeshUtils")
TARGET_LINK_LIBRARIES(lib_MeshUtils Mesh lib_predicates)

IF(NOT WITHOUT_SWIG)
    set(SWG_FILES PyMeshUtils.i)

    # Generate wrapper for python
    SET_SOURCE_FILES_PROPERTIES(${SWG_FILES} PROPERTIES CPLUSPLUS ON)
    SET_SOURCE_FILES_PROPERTIES(${SWG_FILES} PROPERTIES CMAKE_SWIG_FLAGS "-includeall")
    SWIG_ADD_MODULE(PyMeshUtils python ${SWG_FILES} ${INC_FILES})
    SWIG_LINK_LIBRARIES(PyMeshUtils ${PYTHON_LIBRARIES} lib_MeshUtils Mesh
        lib_predicates)

    # Boudle lib, swig wrapper into a single target.
    # Usage: make MeshUtils
    ADD_CUSTOM_TARGET(MeshUtils DEPENDS lib_MeshUtils _PyMeshUtils)
ELSE(NOT WITHOUT_SWIG)
    ADD_CUSTOM_TARGET(MeshUtils DEPENDS lib_MeshUtils)
ENDIF(NOT WITHOUT_SWIG)
