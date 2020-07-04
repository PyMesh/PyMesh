if (TARGET PyMesh::Triangle AND TARGET PyMesh::TetGen)
    file(GLOB SRC_FILES *.cpp)
    file(GLOB INC_FILES *.h)

    add_subdirectory(Attributes)
    add_subdirectory(WireNetwork)
    add_subdirectory(Tiler)
    add_subdirectory(Misc)
    add_subdirectory(Inflator)
    add_subdirectory(Interfaces)
    add_subdirectory(Parameters)

    add_library(lib_Wires SHARED ${SRC_FILES} ${INC_FILES})
    add_library(PyMesh::Wires ALIAS lib_Wires)
    target_link_libraries(lib_Wires
        PUBLIC
            PyMesh::Mesh
            PyMesh::Tools
        PRIVATE
            PyMesh::MeshUtils
            PyMesh::ConvexHull
            PyMesh::Triangle
            PyMesh::Boolean
            PyMesh::TetGen
            PyMesh::BVH
            PyMesh::third_party::json
    )
    set_target_properties(lib_Wires PROPERTIES OUTPUT_NAME "PyMesh-Wires")
endif()
