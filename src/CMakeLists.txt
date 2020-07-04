# Enumerate source files
file(GLOB_RECURSE SRC_FILES *.cpp *.c)
file(GLOB_RECURSE INC_FILES */*.h)

# Add executable and library
add_library(Mesh SHARED ${SRC_FILES} ${INC_FILES})
set_target_properties(Mesh PROPERTIES OUTPUT_NAME "PyMesh-Mesh")
add_library(PyMesh::Mesh ALIAS Mesh)

target_link_libraries(Mesh
    PUBLIC
        PyMesh::third_party::Eigen
        PyMesh::third_party::TBB
)

target_include_directories(Mesh SYSTEM
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Google sparse/dense hash
if (TARGET PyMesh::third_party::SparseHash)
    target_link_libraries(Mesh PRIVATE PyMesh::third_party::SparseHash)
endif ()

