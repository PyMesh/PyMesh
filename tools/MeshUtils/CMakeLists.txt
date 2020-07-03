# Enumerate source files
file(GLOB SRC_FILES *.cpp)
file(GLOB INC_FILES *.h)

add_library(lib_MeshUtils SHARED ${SRC_FILES} ${INC_FILES})
set_target_properties(lib_MeshUtils PROPERTIES OUTPUT_NAME "PyMesh-MeshUtils")
target_link_libraries(lib_MeshUtils
    PUBLIC
        PyMesh::Mesh
        PyMesh::Tools
        PyMesh::Predicates
)

add_library(PyMesh::MeshUtils ALIAS lib_MeshUtils)
