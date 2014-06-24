# Include headers
#INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

# Hash Grid Profiler
ADD_EXECUTABLE(hash_grid_profiler HashGridProfiler.cpp)
TARGET_LINK_LIBRARIES(hash_grid_profiler Mesh)

ADD_CUSTOM_TARGET(other_tests DEPENDS hash_grid_profiler)
