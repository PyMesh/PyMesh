# Enumerate source files
file(GLOB_RECURSE SRC_FILES *.cpp)
file(GLOB_RECURSE INC_FILES *.h *.hh)

add_library(lib_SparseSolver SHARED ${SRC_FILES} ${INC_FILES})
add_library(PyMesh::SparseSolver ALIAS lib_SparseSolver)
set_target_properties(lib_SparseSolver PROPERTIES OUTPUT_NAME
    "PyMesh-SparseSolver")
target_link_libraries(lib_SparseSolver
    PUBLIC
        PyMesh::Mesh
        PyMesh::Tools
)

if (TARGET PyMesh::third_party::Umfpack)
    target_link_libraries(lib_SparseSolver PRIVATE PyMesh::third_party::Umfpack)
endif()

if (TARGET PyMesh::third_party::Cholmod AND TARGET PyMesh::third_party::Metis)
    target_link_libraries(lib_SparseSolver PRIVATE
        PyMesh::third_party::Cholmod
        PyMesh::third_party::Metis)
endif()

