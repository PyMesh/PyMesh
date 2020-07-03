# Source files
file(GLOB SRC_FILES *.cpp)
file(GLOB INC_FILES *.h)

add_library(lib_Tetrahedralization SHARED)
add_library(PyMesh::Tetrahedralization ALIAS lib_Tetrahedralization)
target_link_libraries(lib_Tetrahedralization
    PUBLIC
        Mesh
        PyMesh::Tools
    PRIVATE
        PyMesh::MeshUtils
)
set_target_properties(lib_Tetrahedralization PROPERTIES OUTPUT_NAME
    "PyMesh-Tetrahedronization")

if (TARGET PyMesh::CGAL)
    add_subdirectory(CGAL)
    target_link_libraries(lib_Tetrahedralization PRIVATE PyMesh::CGAL)
endif()

if (TARGET PyMesh::Geogram)
    add_subdirectory(Geogram)
    target_link_libraries(lib_Tetrahedralization PRIVATE PyMesh::Geogram)
endif()

if (TARGET PyMesh::third_party::MMG)
    add_subdirectory(MMG)
    target_link_libraries(lib_Tetrahedralization PRIVATE PyMesh::third_party::MMG)
endif()

if (TARGET PyMesh::third_party::Quartet)
    add_subdirectory(Quartet)
    target_link_libraries(lib_Tetrahedralization PRIVATE
        PyMesh::third_party::Quartet)
endif()

if (TARGET PyMesh::TetGen)
    add_subdirectory(TetGen)
    target_link_libraries(lib_Tetrahedralization PRIVATE PyMesh::TetGen)
endif()

if (TARGET PyMesh::third_party::TetWild)
    add_subdirectory(TetWild)
    target_link_libraries(lib_Tetrahedralization PRIVATE
        PyMesh::third_party::TetWild)
endif()

target_sources(lib_Tetrahedralization PRIVATE ${SRC_FILES} ${INC_FILES})
