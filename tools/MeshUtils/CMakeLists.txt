# Enumerate source files
FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(lib_MeshUtils SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_MeshUtils PROPERTIES OUTPUT_NAME "PyMesh-MeshUtils")
TARGET_LINK_LIBRARIES(lib_MeshUtils
    PUBLIC
        Mesh
        PyMesh::Tools
        PyMesh::Tools::Predicates
)

ADD_LIBRARY(PyMesh::Tools::MeshUtils ALIAS lib_MeshUtils)
