ADD_CUSTOM_TARGET(ToolsTests)

IF (TARGET lib_MeshUtils)
    ADD_SUBDIRECTORY(MeshUtils)
    ADD_DEPENDENCIES(ToolsTests
        MeshUtils_tests
        run_MeshUtils_tests
    )
ENDIF (TARGET lib_MeshUtils)

IF (TARGET lib_Assembler)
    ADD_SUBDIRECTORY(Assembler)
    ADD_DEPENDENCIES(ToolsTests
        Assembler_tests
        run_Assembler_tests
    )
ENDIF (TARGET lib_Assembler)

IF (TARGET lib_TetGen)
    ADD_SUBDIRECTORY(TetGen)
    ADD_DEPENDENCIES(ToolsTests
        TetGen_tests
        run_TetGen_tests
    )
ENDIF (TARGET lib_TetGen)

IF (TARGET lib_CGAL)
    ADD_SUBDIRECTORY(CGAL)
    ADD_DEPENDENCIES(ToolsTests
        CGAL_tests
        run_CGAL_tests
    )
ENDIF (TARGET lib_CGAL)

IF (TARGET lib_Triangle)
    ADD_SUBDIRECTORY(Triangle)
    ADD_DEPENDENCIES(ToolsTests
        Triangle_tests
        run_Triangle_tests
    )
ENDIF (TARGET lib_Triangle)

IF (TARGET lib_ConvexHull)
    ADD_SUBDIRECTORY(ConvexHull)
    ADD_DEPENDENCIES(ToolsTests
        ConvexHull_tests
        run_ConvexHull_tests
    )
ENDIF (TARGET lib_ConvexHull)

IF (TARGET lib_Boolean)
    ADD_SUBDIRECTORY(Boolean)
    ADD_DEPENDENCIES(ToolsTests
        Boolean_tests
        run_Boolean_tests
    )
ENDIF (TARGET lib_Boolean)

IF (TARGET lib_EigenUtils)
    ADD_SUBDIRECTORY(EigenUtils)
    ADD_DEPENDENCIES(ToolsTests
        EigenUtils_tests
        run_EigenUtils_tests
    )
ENDIF (TARGET lib_EigenUtils)

IF (TARGET lib_Predicates)
    ADD_SUBDIRECTORY(Predicates)
    ADD_DEPENDENCIES(ToolsTests
        Predicates_tests
        run_Predicates_tests
    )
ENDIF (TARGET lib_Predicates)

IF (TARGET lib_SparseSolver)
    ADD_SUBDIRECTORY(SparseSolver)
    ADD_DEPENDENCIES(ToolsTests
        SparseSolver_tests
        run_SparseSolver_tests
    )
ENDIF (TARGET lib_SparseSolver)

#IF (TARGET envelope)
#    ADD_SUBDIRECTORY(Envelope)
#    LIST(APPEND TARGET_MODULE_TESTS envelope_tests)
#ENDIF (TARGET envelope)

IF (TARGET lib_Wires)
    ADD_SUBDIRECTORY(Wires)
    ADD_DEPENDENCIES(ToolsTests
        Wires_tests
        run_Wires_tests
    )
ENDIF (TARGET lib_Wires)

IF (TARGET lib_IGL)
    ADD_SUBDIRECTORY(IGL)
    ADD_DEPENDENCIES(ToolsTests
        IGL_tests
        run_IGL_tests
    )
ENDIF (TARGET lib_IGL)

IF (TARGET lib_Triangulation)
    ADD_SUBDIRECTORY(Triangulation)
    ADD_DEPENDENCIES(ToolsTests
        Triangulation_tests
        run_Triangulation_tests
    )
ENDIF (TARGET lib_Triangulation)

IF (TARGET lib_Tetrahedralization)
    ADD_SUBDIRECTORY(Tetrahedralization)
    LIST(APPEND TARGET_MODULE_TESTS tetrahedralization_tests)
ENDIF (TARGET lib_Tetrahedralization)

IF (TARGET lib_BVH)
    ADD_SUBDIRECTORY(BVH)
    ADD_DEPENDENCIES(ToolsTests
        BVH_tests
        run_BVH_tests
    )
ENDIF (TARGET lib_BVH)

IF (TARGET lib_Geogram)
    ADD_SUBDIRECTORY(Geogram)
    ADD_DEPENDENCIES(ToolsTests
        Geogram_tests
        run_Geogram_tests
    )
ENDIF (TARGET lib_Geogram)

IF (TARGET lib_Compression)
    ADD_SUBDIRECTORY(Compression)
    ADD_DEPENDENCIES(ToolsTests
        Compression_tests
        run_Compression_tests
    )
ENDIF (TARGET lib_Compression)
