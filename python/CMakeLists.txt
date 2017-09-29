# Add python binding.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tools)
INCLUDE_DIRECTORIES(${EIGEN_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${EIGEN_INCLUDE_DIRS}/unsupported)

INCLUDE_DIRECTORIES(${LIBIGL_INCLUDE_DIRS})
INCLUDE(${CGAL_USE_FILE})
LIST(APPEND DEPENDENT_LIBS ${CGAL_LIBRARIES})

PYBIND11_ADD_MODULE(PyMesh module.cpp
    PyAABB.cpp
    PyBoolean.cpp
    PyCellPartition.cpp
    PyCGAL.cpp
    PyConvexHull.cpp
    PyDMAT.cpp
    PyMeshUtils.cpp
    PyMesh.cpp
    PyMeshFactory.cpp
    PyMeshWriter.cpp
    PyOuterHull.cpp
    PyPredicates.cpp
    PySelfIntersectionResolver.cpp
    PyTetrahedralization.cpp
    PyTriangulation.cpp
    PyMinkowskiSum.cpp
    PyWindingNumber.cpp)
TARGET_LINK_LIBRARIES(PyMesh PRIVATE
    Mesh
    MeshUtils
    cgal_wrapper
    lib_igl
    lib_predicates
    lib_convex_hull
    lib_self_intersection
    lib_boolean
    lib_outer_hull
    lib_tetrahedronization
    lib_triangulation
    lib_winding_number)
