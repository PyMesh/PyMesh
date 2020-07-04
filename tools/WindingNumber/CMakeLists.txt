# Enumerate source files
file(GLOB SRC_FILES *.cpp *.inl)
file(GLOB INC_FILES *.h)

add_library(lib_WindingNumber SHARED)
add_library(PyMesh::WindingNumber ALIAS lib_WindingNumber)
set_target_properties(lib_WindingNumber PROPERTIES OUTPUT_NAME
    "PyMesh-WindingNumber")
target_link_libraries(lib_WindingNumber
    PUBLIC
        PyMesh::Mesh
        PyMesh::Tools
)

if (TARGET PyMesh::IGL)
    add_subdirectory(IGL)
    target_link_libraries(lib_WindingNumber PRIVATE PyMesh::IGL)
endif()

if (TARGET PyMesh::third_party::FastWindingNumber)
    add_subdirectory(FastWindingNumber)
    target_link_libraries(lib_WindingNumber PRIVATE
        PyMesh::third_party::FastWindingNumber)
endif()

target_sources(lib_WindingNumber PRIVATE ${SRC_FILES} ${INC_FILES})
