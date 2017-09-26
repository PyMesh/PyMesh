# Add python binding.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tools)
INCLUDE_DIRECTORIES(${EIGEN_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${EIGEN_INCLUDE_DIRS}/unsupported)
PYBIND11_ADD_MODULE(PyMesh module.cpp
    PyBoolean.cpp
    PyConvexHull.cpp
    PyMeshUtils.cpp
    PyMesh.cpp
    PyMeshFactory.cpp
    PyMeshWriter.cpp
    PyOuterHull.cpp
    PyPredicates.cpp
    PySelfIntersectionResolver.cpp
    PyTetrahedralization.cpp)
TARGET_LINK_LIBRARIES(PyMesh PRIVATE
    Mesh
    MeshUtils
    lib_predicates
    lib_convex_hull
    lib_self_intersection
    lib_boolean
    lib_outer_hull
    lib_tetrahedronization)
