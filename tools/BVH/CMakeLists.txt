# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

ADD_SUBDIRECTORY(CGAL)
ADD_SUBDIRECTORY(Geogram)
ADD_SUBDIRECTORY(IGL)

ADD_LIBRARY(lib_BVH SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_BVH PROPERTIES OUTPUT_NAME "PyMesh-BVH")
TARGET_LINK_LIBRARIES(lib_BVH
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        PyMesh::Tools::CGAL
        PyMesh::Tools::Geogram
        PyMesh::Tools::IGL
)

ADD_LIBRARY(PyMesh::Tools::BVH ALIAS lib_BVH)
