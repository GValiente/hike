cmake_minimum_required(VERSION 3.5)
project(hike-tests)

# Enable C++11:
set(CMAKE_CXX_STANDARD 11)

# Define sources:
set(SOURCES
    src/vns_example_test.cpp
    src/cached_loss_function_example_test.cpp
    src/parallel_bi_local_search_example_test.cpp
    src/on_improved_solution_example_test.cpp
    src/one_dim_vns_tests.cpp
    src/two_dim_vns_tests.cpp
    src/three_dim_vns_tests.cpp
    src/thread_pool_tests.cpp
)

# Add a executable with the above sources:
add_executable(${PROJECT_NAME} ${SOURCES})

# Define include directories:
target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

# Link static libraries:
target_link_libraries(${PROJECT_NAME}
    hike
)

# Include catch library:
add_subdirectory(3rd_party/catch)
target_link_libraries(${PROJECT_NAME} catch)

# Include pthread (GCC only) (if you don't need ParallelBILocalSearch, you can remove this dependency):
if(CMAKE_COMPILER_IS_GNUCC)
    target_link_libraries(${PROJECT_NAME} pthread)
endif()
