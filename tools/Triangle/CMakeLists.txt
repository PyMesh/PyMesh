SET(SRC_FILES TriangleWrapper.cpp)
SET(INC_FILES TriangleWrapper.h)

ADD_LIBRARY(lib_Triangle SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_Triangle PROPERTIES OUTPUT_NAME "PyMesh-Triangle")
TARGET_LINK_LIBRARIES(lib_Triangle 
    PUBLIC
        Mesh
        PyMesh::Tools
        PyMesh::Triangle
    PRIVATE
        PyMesh::Tools::MeshUtils
)

ADD_LIBRARY(PyMesh::Tools::Triangle ALIAS lib_Triangle)
