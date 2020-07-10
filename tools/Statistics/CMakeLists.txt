# Enumerate source files
file(GLOB SRC_FILES *.cpp *.inl)
file(GLOB INC_FILES *.h)

add_library(lib_Statistics SHARED)
set_target_properties(lib_Statistics PROPERTIES OUTPUT_NAME "PyMesh-Statistics")
add_library(PyMesh::Statistics ALIAS lib_Statistics)

target_link_libraries(lib_Statistics PUBLIC PyMesh::Mesh PyMesh::Tools)

if (TARGET PyMesh::CGAL)
    add_subdirectory(CGAL)
    target_link_libraries(lib_Statistics PUBLIC PyMesh::CGAL)
endif()

target_sources(lib_Statistics PRIVATE ${SRC_FILES} ${INC_FILES})
