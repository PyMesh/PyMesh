# Include headers
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tools)

ADD_SUBDIRECTORY(opencl)
ADD_SUBDIRECTORY(swig_eigen)
ADD_SUBDIRECTORY(eigen)
ADD_SUBDIRECTORY(swig)

ADD_CUSTOM_TARGET(learning_tests DEPENDS opencl_tests swig_eigen_tests
    eigen_tests swig_tests)
