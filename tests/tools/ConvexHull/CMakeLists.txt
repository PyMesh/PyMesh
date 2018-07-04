# Enumerate source files
FILE(GLOB_RECURSE TEST_SRC_FILES *.cpp)
FILE(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

ADD_EXECUTABLE(ConvexHull_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(ConvexHull_tests
    PRIVATE
        PyMesh::UnitTest
        lib_ConvexHull
)
TARGET_INCLUDE_DIRECTORIES(ConvexHull_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
ADD_CUSTOM_TARGET(run_ConvexHull_tests
    DEPENDS
        ConvexHull_tests
    COMMAND
        ConvexHull_tests
)

# CGAL
IF (CGAL_FOUND)
    TARGET_COMPILE_DEFINITIONS(ConvexHull_tests
        PRIVATE
            -DWITH_CGAL
    )
ENDIF (CGAL_FOUND)

# Include Qhull
IF (QHULL_FOUND)
    TARGET_COMPILE_DEFINITIONS(ConvexHull_tests
        PRIVATE
            -DWITH_QHULL
    )
ENDIF (QHULL_FOUND)

# Include Triangle
IF (TRIANGLE_FOUND)
    TARGET_COMPILE_DEFINITIONS(ConvexHull_tests
        PRIVATE
            -DWITH_TRIANGLE
    )
ENDIF (TRIANGLE_FOUND)

