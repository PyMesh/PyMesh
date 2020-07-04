# Enumerate source files
file(GLOB SRC_FILES *.cpp *.inl)
file(GLOB INC_FILES *.h)

add_library(lib_Compression SHARED)
add_library(PyMesh::Compression ALIAS lib_Compression)
set_target_properties(lib_Compression PROPERTIES OUTPUT_NAME
    "PyMesh-Compression")
target_link_libraries(lib_Compression
    PUBLIC
        PyMesh::Mesh
        PyMesh::Tools
)

if (TARGET PyMesh::third_party::Draco)
    add_subdirectory(Draco)
    target_link_libraries(lib_Compression PUBLIC PyMesh::third_party::Draco)
endif()

target_sources(lib_Compression PRIVATE ${SRC_FILES} ${INC_FILES})
