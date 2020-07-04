add_custom_target(ToolsTests)

if (TARGET PyMesh::MeshUtils)
    add_subdirectory(MeshUtils)
    add_dependencies(ToolsTests
        MeshUtils_tests
        run_MeshUtils_tests
    )
endif ()

if (TARGET PyMesh::Assembler)
    add_subdirectory(Assembler)
    add_dependencies(ToolsTests
        Assembler_tests
        run_Assembler_tests
    )
endif ()

if (TARGET PyMesh::TetGen)
    add_subdirectory(TetGen)
    add_dependencies(ToolsTests
        TetGen_tests
        run_TetGen_tests
    )
endif ()

if (TARGET PyMesh::CGAL)
    add_subdirectory(CGAL)
    add_dependencies(ToolsTests
        CGAL_tests
        run_CGAL_tests
    )
endif ()

if (TARGET PyMesh::Triangle)
    add_subdirectory(Triangle)
    add_dependencies(ToolsTests
        Triangle_tests
        run_Triangle_tests
    )
endif ()

if (TARGET PyMesh::ConvexHull)
    add_subdirectory(ConvexHull)
    add_dependencies(ToolsTests
        ConvexHull_tests
        run_ConvexHull_tests
    )
endif ()

if (TARGET PyMesh::Boolean)
    add_subdirectory(Boolean)
    add_dependencies(ToolsTests
        Boolean_tests
        run_Boolean_tests
    )
endif ()

if (TARGET PyMesh::Predicates)
    add_subdirectory(Predicates)
    add_dependencies(ToolsTests
        Predicates_tests
        run_Predicates_tests
    )
endif ()

if (TARGET PyMesh::SparseSolver)
    add_subdirectory(SparseSolver)
    add_dependencies(ToolsTests
        SparseSolver_tests
        run_SparseSolver_tests
    )
endif ()

if (TARGET PyMesh::Wires)
    add_subdirectory(Wires)
    add_dependencies(ToolsTests
        Wires_tests
        run_Wires_tests
    )
endif ()

if (TARGET PyMesh::IGL)
    add_subdirectory(IGL)
    add_dependencies(ToolsTests
        IGL_tests
        run_IGL_tests
    )
endif ()

if (TARGET PyMesh::Triangulation)
    add_subdirectory(Triangulation)
    add_dependencies(ToolsTests
        Triangulation_tests
        run_Triangulation_tests
    )
endif ()

if (TARGET PyMesh::Tetrahedralization)
    add_subdirectory(Tetrahedralization)
    list(APPEND TARGET_MODULE_TESTS tetrahedralization_tests)
endif ()

if (TARGET PyMesh::BVH)
    add_subdirectory(BVH)
    add_dependencies(ToolsTests
        BVH_tests
        run_BVH_tests
    )
endif ()

if (TARGET PyMesh::Geogram)
    add_subdirectory(Geogram)
    add_dependencies(ToolsTests
        Geogram_tests
        run_Geogram_tests
    )
endif ()

if (TARGET PyMesh::Compression)
    add_subdirectory(Compression)
    add_dependencies(ToolsTests
        Compression_tests
        run_Compression_tests
    )
endif ()
