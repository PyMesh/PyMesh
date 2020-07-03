# Enumerate source files
file(GLOB SRC_FILES *.cpp *.inl)
file(GLOB INC_FILES *.h)

add_library(lib_SelfIntersection SHARED)
add_library(PyMesh::SelfIntersection ALIAS lib_SelfIntersection)
set_target_properties(lib_SelfIntersection PROPERTIES OUTPUT_NAME
    "PyMesh-SelfIntersection")
target_link_libraries(lib_SelfIntersection
    PUBLIC
        PyMesh::Tools
        PyMesh::Mesh
)

if (TARGET PyMesh::IGL)
    add_subdirectory(IGL)
    target_link_libraries(lib_SelfIntersection PRIVATE PyMesh::IGL)
endif()

target_sources(lib_SelfIntersection PRIVATE ${INC_FILES} ${SRC_FILES})
