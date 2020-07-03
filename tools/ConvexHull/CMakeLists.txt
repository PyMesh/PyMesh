# Source files
file(GLOB SRC_FILES *.cpp)
file(GLOB INC_FILES *.h)

add_library(lib_ConvexHull SHARED)
add_library(PyMesh::ConvexHull ALIAS lib_ConvexHull)
set_target_properties(lib_ConvexHull PROPERTIES OUTPUT_NAME
    "PyMesh-ConvexHull")
target_link_libraries(lib_ConvexHull PUBLIC PyMesh::Tools PyMesh::MeshUtils)

if (TARGET PyMesh::third_party::Qhull)
    add_subdirectory(Qhull)
    target_link_libraries(lib_ConvexHull PUBLIC PyMesh::third_party::Qhull)
endif ()

if (TARGET PyMesh::CGAL)
    add_subdirectory(CGAL)
    target_link_libraries(lib_ConvexHull PUBLIC PyMesh::CGAL)
endif ()

if (TARGET PyMesh::TetGen)
    add_subdirectory(TetGen)
    target_link_libraries(lib_ConvexHull PUBLIC PyMesh::TetGen)
endif ()

if (TARGET PyMesh::Triangle)
    add_subdirectory(Triangle)
    target_link_libraries(lib_ConvexHull PUBLIC PyMesh::Triangle)
endif ()

target_sources(lib_ConvexHull PRIVATE ${SRC_FILES} ${INC_FILES})
