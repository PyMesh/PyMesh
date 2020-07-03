# Enumerate source files
file(GLOB SRC_FILES *.cpp *.inl)
file(GLOB INC_FILES *.h)

add_library(lib_BVH SHARED)
add_library(PyMesh::BVH ALIAS lib_BVH)
set_target_properties(lib_BVH PROPERTIES OUTPUT_NAME "PyMesh-BVH")
target_link_libraries(lib_BVH
    PUBLIC
        PyMesh::Mesh
        PyMesh::Tools
)
if (WIN32)
    TARGET_COMPILE_DEFINITIONS(lib_BVH PRIVATE -D_ENABLE_EXTENDED_ALIGNED_STORAGE)
endif (WIN32)

if (TARGET PyMesh::CGAL)
    add_subdirectory(CGAL)
    target_link_libraries(lib_BVH PRIVATE PyMesh::CGAL)
endif()

if (TARGET PyMesh::Geogram)
    add_subdirectory(Geogram)
    target_link_libraries(lib_BVH PRIVATE PyMesh::Geogram)
endif()

if (TARGET PyMesh::IGL)
    add_subdirectory(IGL)
    target_link_libraries(lib_BVH PRIVATE PyMesh::IGL)
endif()

target_sources(lib_BVH PRIVATE ${SRC_FILES} ${INC_FILES})
