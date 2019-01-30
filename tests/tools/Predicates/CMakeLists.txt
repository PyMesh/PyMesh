# Enumerate source files
FILE(GLOB TEST_SRC_FILES unit_test_driver.cpp)
FILE(GLOB TEST_INC_FILES *_test.h)

ADD_EXECUTABLE(Predicates_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(Predicates_tests 
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Tools::Predicates
)
ADD_CUSTOM_TARGET(run_Predicates_tests
    DEPENDS
        Predicates_tests
    COMMAND
        Predicates_tests
)
