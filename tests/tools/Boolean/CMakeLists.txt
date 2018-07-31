# Enumerate source files
FILE(GLOB_RECURSE TEST_SRC_FILES *.cpp)
FILE(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

ADD_EXECUTABLE(Boolean_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(Boolean_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Tools::Boolean
)
ADD_CUSTOM_TARGET(run_Boolean_tests
    DEPENDS
        Boolean_tests
    COMMAND
        Boolean_tests
)

