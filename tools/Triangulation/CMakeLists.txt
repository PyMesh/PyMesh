# Enumerate source files
file(GLOB SRC_FILES *.cpp *.inl)
file(GLOB INC_FILES *.h)

add_library(lib_Triangulation SHARED)
add_library(PyMesh::Triangulation ALIAS lib_Triangulation)
set_target_properties(lib_Triangulation PROPERTIES OUTPUT_NAME
    "PyMesh-Triangulation")
target_link_libraries(lib_Triangulation
    PUBLIC
        PyMesh::Mesh
        PyMesh::Tools
)

if (TARGET PyMesh::IGL)
    add_subdirectory(IGL)
    target_link_libraries(lib_Triangulation PRIVATE PyMesh::IGL)
endif()

if (TARGET PyMesh::CGAL)
    add_subdirectory(CGAL)
    target_link_libraries(lib_Triangulation PRIVATE PyMesh::CGAL)
endif()

if (TARGET PyMesh::Triangle)
    add_subdirectory(ShewchukTriangle)
    target_link_libraries(lib_Triangulation PRIVATE PyMesh::Triangle)
endif()

if (TARGET PyMesh::Geogram)
    add_subdirectory(Geogram)
    target_link_libraries(lib_Triangulation PRIVATE PyMesh::Geogram)
endif()

if (TARGET PyMesh::third_party::Jigsaw)
    add_subdirectory(Jigsaw)
    target_link_libraries(lib_Triangulation PRIVATE PyMesh::third_party::Jigsaw)
endif()

if (TARGET PyMesh::third_party::MMG)
    add_subdirectory(MMG)
    target_link_libraries(lib_Triangulation PRIVATE PyMesh::third_party::MMG)
endif()

target_sources(lib_Triangulation PRIVATE ${SRC_FILES} ${INC_FILES})
