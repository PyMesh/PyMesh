PYBIND11_ADD_MODULE(PyMesh module.cpp
    PyAABB.cpp
    PyBoolean.cpp
    PyCellPartition.cpp
    PyCGAL.cpp
    PyCompression.cpp
    PyConvexHull.cpp
    PyDMAT.cpp
    PyFEM.cpp
    PyGeogram.cpp
    PyHashGrid.cpp
    PyIGL.cpp
    PyMeshUtils.cpp
    PyMesh.cpp
    PyMeshFactory.cpp
    PyMeshWriter.cpp
    PyOuterHull.cpp
    PyPredicates.cpp
    PySelfIntersectionResolver.cpp
    PySparseSolver.cpp
    PyTetgen.cpp
    PyTetrahedralization.cpp
    PyTriangle.cpp
    PyTriangulation.cpp
    PyMinkowskiSum.cpp
    PyVoxelGrid.cpp
    PyWindingNumber.cpp
    PyWires.cpp)
target_link_libraries(PyMesh PRIVATE
    PyMesh::Mesh
    PyMesh::Assembler
    PyMesh::Envelope
    PyMesh::MeshUtils
    PyMesh::Predicates
    PyMesh::Compression
    PyMesh::ConvexHull
    PyMesh::SelfIntersection
    PyMesh::Boolean
    PyMesh::BVH
    PyMesh::OuterHull
    PyMesh::SparseSolver
    PyMesh::Tetrahedralization
    PyMesh::Triangulation
    PyMesh::WindingNumber
    PyMesh::Wires)

if (TARGET PyMesh::CGAL)
    target_link_libraries(PyMesh PRIVATE PyMesh::CGAL)
endif()

if (TARGET PyMesh::IGL)
    target_link_libraries(PyMesh PRIVATE PyMesh::IGL)
endif()

if (TARGET PyMesh::Geogram)
    target_link_libraries(PyMesh PRIVATE PyMesh::Geogram)
endif()

if (TARGET PyMesh::Triangle)
    target_link_libraries(PyMesh PRIVATE PyMesh::Triangle)
endif()

if (TARGET PyMesh::TetGen)
    target_link_libraries(PyMesh PRIVATE PyMesh::TetGen)
endif()

target_include_directories(PyMesh SYSTEM PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/tools
    ${EIGEN_INCLUDE_DIRS}
    ${EIGEN_INCLUDE_DIRS}/unsupported
    ${LIBIGL_INCLUDE_DIRS})
