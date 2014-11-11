INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/tools)

# OpenCL
IF (OPENCL_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
    INCLUDE_DIRECTORIES(${OPENCL_INCLUDE_DIRS})
ENDIF(OPENCL_FOUND)

FILE(GLOB SRC_FILES *.cpp *.c)
FILE(GLOB INC_FILES *.h)

ADD_LIBRARY(lib_opencl_wrapper SHARED ${SRC_FILES} ${INC_FILES})
TARGET_LINK_LIBRARIES(lib_opencl_wrapper ${OPENCL_LIBRARIES})
SET_TARGET_PROPERTIES(lib_opencl_wrapper PROPERTIES OUTPUT_NAME "opencl_wrapper")

# Boudle lib into a single target.
# To build it, one can just "make opencl_wrapper"
ADD_CUSTOM_TARGET(opencl_wrapper DEPENDS lib_opencl_wrapper)
