# Enumerate source files
FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(lib_CGAL SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_CGAL PROPERTIES OUTPUT_NAME "PyMesh-CGAL")
TARGET_LINK_LIBRARIES(lib_CGAL
    PUBLIC
        Mesh
        PyMesh::Tools
        PyMesh::CGAL
        PyMesh::GMP
        PyMesh::MPFR
)

ADD_LIBRARY(PyMesh::Tools::CGAL ALIAS lib_CGAL)
