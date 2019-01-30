# Enumerate source files
FILE(GLOB_RECURSE TEST_SRC_FILES *.cpp)
FILE(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

ADD_EXECUTABLE(Assembler_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(Assembler_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Tools::Assembler
)
ADD_CUSTOM_TARGET(run_Assembler_tests
    COMMAND
        Assembler_tests
    DEPENDS
        Assembler_tests
)
