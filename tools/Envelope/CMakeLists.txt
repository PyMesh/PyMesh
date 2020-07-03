# Include this directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Enumerate source files
file(GLOB SRC_FILES *.cpp *.inl)
file(GLOB INC_FILES *.h)

add_library(lib_envelope INTERFACE)
add_library(PyMesh::Envelope ALIAS lib_envelope)
target_link_libraries(lib_envelope INTERFACE PyMesh::Tools PyMesh::Mesh)
