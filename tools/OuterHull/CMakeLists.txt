# Enumerate source files
file(GLOB SRC_FILES *.cpp *.inl)
file(GLOB INC_FILES *.h)

add_library(lib_OuterHull SHARED)
add_library(PyMesh::OuterHull ALIAS lib_OuterHull)
set_target_properties(lib_OuterHull PROPERTIES OUTPUT_NAME "PyMesh-OuterHull")
target_link_libraries(lib_OuterHull
    PUBLIC
        PyMesh::Mesh
        PyMesh::Tools
    PRIVATE
        PyMesh::MeshUtils
)

if (TARGET PyMesh::IGL AND TARGET PyMesh::SelfIntersection)
    add_subdirectory(IGL)
    target_link_libraries(lib_OuterHull
        PRIVATE
            PyMesh::IGL
            PyMesh::SelfIntersection
    )
endif()

target_sources(lib_OuterHull PRIVATE ${INC_FILES} ${SRC_FILES})
