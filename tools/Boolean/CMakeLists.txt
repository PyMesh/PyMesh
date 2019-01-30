# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

ADD_SUBDIRECTORY(Cork)
ADD_SUBDIRECTORY(Clipper)
ADD_SUBDIRECTORY(IGL)
ADD_SUBDIRECTORY(CGAL)
ADD_SUBDIRECTORY(Carve)
ADD_SUBDIRECTORY(BSP)

ADD_LIBRARY(lib_Boolean SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_Boolean PROPERTIES OUTPUT_NAME "PyMesh-Boolean")
TARGET_LINK_LIBRARIES(lib_Boolean
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        lib_MeshUtils
        PyMesh::Cork
        PyMesh::Clipper
        PyMesh::Carve
        PyMesh::BSP
        PyMesh::TinyXML2
        PyMesh::Tools::Triangle
        PyMesh::Tools::IGL
        PyMesh::Tools::CGAL
)

ADD_LIBRARY(PyMesh::Tools::Boolean ALIAS lib_Boolean)
