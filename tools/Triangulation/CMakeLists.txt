# Enumerate source files
FILE(GLOB SRC_FILES *.cpp *.inl)
FILE(GLOB INC_FILES *.h)

ADD_SUBDIRECTORY(IGL)
ADD_SUBDIRECTORY(CGAL)
ADD_SUBDIRECTORY(ShewchukTriangle)
ADD_SUBDIRECTORY(Geogram)

ADD_LIBRARY(lib_Triangulation SHARED ${SRC_FILES} ${INC_FILES})
SET_TARGET_PROPERTIES(lib_Triangulation PROPERTIES OUTPUT_NAME
    "PyMesh-Triangulation")
TARGET_LINK_LIBRARIES(lib_Triangulation
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        PyMesh::Tools::IGL
        PyMesh::Tools::CGAL
        PyMesh::Tools::Triangle
        PyMesh::Tools::Geogram
)

ADD_LIBRARY(PyMesh::Tools::Triangulation ALIAS lib_Triangulation)
