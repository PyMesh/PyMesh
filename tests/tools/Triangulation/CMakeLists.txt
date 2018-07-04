# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ADD_EXECUTABLE(Triangulation_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(Triangulation_tests
    PRIVATE
        PyMesh::UnitTest
        lib_MeshUtils
        lib_Triangulation
)
ADD_CUSTOM_TARGET(run_Triangulation_tests 
    DEPENDS
        Triangulation_tests
    COMMAND
        Triangulation_tests
)

# Include IGL
IF (LIBIGL_FOUND AND CGAL_FOUND)
    TARGET_COMPILE_DEFINITIONS(Triangulation_tests
        PRIVATE
            -DWITH_IGL
            -DWITH_CGAL
    )
ENDIF (LIBIGL_FOUND AND CGAL_FOUND)

# Include CGAL
IF (CGAL_FOUND)
    TARGET_COMPILE_DEFINITIONS(Triangulation_tests
        PRIVATE
            -DWITH_CGAL
    )
ENDIF (CGAL_FOUND)

# Include Triangle
IF (TRIANGLE_FOUND)
    TARGET_COMPILE_DEFINITIONS(Triangulation_tests
        PRIVATE
            -DWITH_TRIANGLE
    )
ENDIF (TRIANGLE_FOUND)

# Include Geogram
IF (GEOGRAM_FOUND)
    TARGET_COMPILE_DEFINITIONS(Triangulation_tests
        PRIVATE
            -DWITH_GEOGRAM
    )
ENDIF (GEOGRAM_FOUND)

