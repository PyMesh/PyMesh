# Enumerate source files
FILE(GLOB_RECURSE TEST_SRC_FILES *.cpp)
FILE(GLOB_RECURSE TEST_INC_FILES *.h *.inl)

ADD_EXECUTABLE(Wires_tests ${TEST_SRC_FILES} ${TEST_INC_FILES})
TARGET_LINK_LIBRARIES(Wires_tests
    PRIVATE
        PyMesh::UnitTest
        PyMesh::Tools::Wires
        PyMesh::Tools::BVH
)
TARGET_INCLUDE_DIRECTORIES(Wires_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
ADD_CUSTOM_TARGET(run_Wires_tests
    DEPENDS
        Wires_tests
    COMMAND 
        Wires_tests
)
