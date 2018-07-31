# Source files
FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)

ADD_SUBDIRECTORY(Qhull)
ADD_SUBDIRECTORY(CGAL)
ADD_SUBDIRECTORY(TetGen)
ADD_SUBDIRECTORY(Triangle)

ADD_LIBRARY(lib_ConvexHull SHARED ${SRC_FILES} ${INC_FILES})
TARGET_LINK_LIBRARIES(lib_ConvexHull
    PUBLIC
        PyMesh::Tools
    PRIVATE
        lib_MeshUtils
        PyMesh::Qhull
        PyMesh::Tools::CGAL
        PyMesh::Tools::Triangle
        PyMesh::Tools::TetGen
)
SET_TARGET_PROPERTIES(lib_ConvexHull PROPERTIES OUTPUT_NAME
    "PyMesh-ConvexHull")

ADD_LIBRARY(PyMesh::Tools::ConvexHull ALIAS lib_ConvexHull)
