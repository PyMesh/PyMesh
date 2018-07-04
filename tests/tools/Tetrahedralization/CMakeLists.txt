# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ADD_EXECUTABLE(Tetrahedralization_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(Tetrahedralization_tests
    PRIVATE
        lib_Tetrahedralization
        PyMesh::UnitTest
)
ADD_CUSTOM_TARGET(run_Tetrahedralization_tests
    DEPENDS
        Tetrahedralization_tests
    COMMAND
        Tetrahedralization_tests
)

# Inlclude CGAL
IF(CGAL_FOUND)
    TARGET_COMPILE_DEFINITIONS(Tetrahedralization_tests
        PRIVATE
            -DWITH_CGAL
            -DCGAL_CONCURRENT_MESH_3
    )
ENDIF(CGAL_FOUND)

# Include TetGen
IF(TETGEN_FOUND)
    TARGET_COMPILE_DEFINITIONS(Tetrahedralization_tests
        PRIVATE
            -DWITH_TETGEN
    )
ENDIF(TETGEN_FOUND)

# Include GeoGram
IF (GEOGRAM_FOUND)
    TARGET_COMPILE_DEFINITIONS(Tetrahedralization_tests
        PRIVATE
            -DWITH_GEOGRAM
    )
ENDIF (GEOGRAM_FOUND)

# Include Quartet
IF (QUARTET_FOUND)
    TARGET_COMPILE_DEFINITIONS(Tetrahedralization_tests
        PRIVATE
            -DWITH_QUARTET
    )
ENDIF (QUARTET_FOUND)

# TetWild
IF (TETWILD_FOUND)
    TARGET_COMPILE_DEFINITIONS(Tetrahedralization_tests
        PRIVATE
            -DWITH_TETWILD
    )
ENDIF(TETWILD_FOUND)
