# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

ADD_SUBDIRECTORY(IGL)
ADD_LIBRARY(lib_OuterHull SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_OuterHull PROPERTIES OUTPUT_NAME "PyMesh-OuterHull")
TARGET_LINK_LIBRARIES(lib_OuterHull
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        PyMesh::Tools::MeshUtils
        PyMesh::Tools::IGL
        PyMesh::Tools::SelfIntersection
)

ADD_LIBRARY(PyMesh::Tools::OuterHull ALIAS lib_OuterHull)
