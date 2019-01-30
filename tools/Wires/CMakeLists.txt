FILE(GLOB SRC_FILES *.cpp)
FILE(GLOB INC_FILES *.h)
FILE(GLOB DEP_FILES *.i)

ADD_SUBDIRECTORY(Attributes)
ADD_SUBDIRECTORY(WireNetwork)
ADD_SUBDIRECTORY(Tiler)
ADD_SUBDIRECTORY(Misc)
ADD_SUBDIRECTORY(Inflator)
ADD_SUBDIRECTORY(Interfaces)
ADD_SUBDIRECTORY(Parameters)

ADD_LIBRARY(lib_Wires SHARED ${SRC_FILES} ${INC_FILES})
TARGET_LINK_LIBRARIES(lib_Wires
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        PyMesh::Tools::MeshUtils
        PyMesh::Tools::ConvexHull
        PyMesh::Tools::Triangle
        PyMesh::Tools::Boolean
        PyMesh::Tools::TetGen
        PyMesh::Tools::BVH
        Boost::boost
)
SET_TARGET_PROPERTIES(lib_Wires PROPERTIES OUTPUT_NAME "PyMesh-Wires")

ADD_LIBRARY(PyMesh::Tools::Wires ALIAS lib_Wires)
