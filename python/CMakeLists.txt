# Add python binding.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tools)
INCLUDE_DIRECTORIES(${EIGEN_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${EIGEN_INCLUDE_DIRS}/unsupported)
PYBIND11_ADD_MODULE(PyMesh PyMesh.cpp PyMeshUtils.cpp Mesh.cpp MeshFactory.cpp
    MeshWriter.cpp PyPredicates.cpp ConvexHull.cpp)
TARGET_LINK_LIBRARIES(PyMesh PRIVATE Mesh MeshUtils lib_predicates
    lib_convex_hull)
SET_TARGET_PROPERTIES(PyMesh PROPERTIES LINKER_LANGUAGE CXX)
