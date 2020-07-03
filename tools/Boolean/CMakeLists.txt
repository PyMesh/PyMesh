# Enumerate source files
file(GLOB SRC_FILES *.cpp *.inl)
file(GLOB INC_FILES *.h)

add_library(lib_Boolean SHARED)
set_target_properties(lib_Boolean PROPERTIES OUTPUT_NAME "PyMesh-Boolean")
add_library(PyMesh::Boolean ALIAS lib_Boolean)

target_link_libraries(lib_Boolean PUBLIC PyMesh::Mesh PyMesh::Tools)

if (TARGET PyMesh::third_party::Cork)
    add_subdirectory(Cork)
    target_link_libraries(lib_Boolean PRIVATE PyMesh::third_party::Cork)
endif()

if (TARGET PyMesh::third_party::Clipper AND
        TARGET PyMesh::Triangle)
    add_subdirectory(Clipper)
    target_link_libraries(lib_Boolean PRIVATE
        PyMesh::third_party::Clipper
        PyMesh::Triangle)
endif()

if (TARGET PyMesh::IGL)
    add_subdirectory(IGL)
    target_link_libraries(lib_Boolean PUBLIC PyMesh::IGL)
endif()

if (TARGET PyMesh::CGAL)
    add_subdirectory(CGAL)
    target_link_libraries(lib_Boolean PUBLIC PyMesh::CGAL)
endif()

if (TARGET PyMesh::third_party::Carve)
    add_subdirectory(Carve)
    target_link_libraries(lib_Boolean PRIVATE PyMesh::third_party::Carve)
endif()

if (TARGET PyMesh::third_party::BSP)
    add_subdirectory(BSP)
    target_link_libraries(lib_Boolean PRIVATE PyMesh::third_party::BSP)
endif()

target_sources(lib_Boolean PRIVATE ${SRC_FILES} ${INC_FILES})
