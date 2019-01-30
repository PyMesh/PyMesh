# Source files
FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

ADD_SUBDIRECTORY(CGAL)
ADD_SUBDIRECTORY(Geogram)
ADD_SUBDIRECTORY(MMG)
ADD_SUBDIRECTORY(Quartet)
ADD_SUBDIRECTORY(TetGen)
ADD_SUBDIRECTORY(TetWild)

ADD_LIBRARY(lib_Tetrahedralization SHARED ${SRC_FILES} ${INC_FILES})
TARGET_LINK_LIBRARIES(lib_Tetrahedralization
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        PyMesh::Tools::MeshUtils
        PyMesh::Tools::CGAL
        PyMesh::Tools::TetGen
        PyMesh::Tools::Geogram
        PyMesh::Quartet
        PyMesh::MMG
        PyMesh::TetWild
)
SET_TARGET_PROPERTIES(lib_Tetrahedralization PROPERTIES OUTPUT_NAME
    "PyMesh-Tetrahedronization")

ADD_LIBRARY(PyMesh::Tools::Tetrahedralization ALIAS lib_Tetrahedralization)
