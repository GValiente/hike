cmake_minimum_required(VERSION 3.5)
project(hike-project)

# Add library subdirectory:
add_subdirectory(lib)

# Define build all artefacts option:
option(HIKE_BUILD_ALL "Build all hike artefacts" OFF)

# Add tests subdirectory:
option(HIKE_BUILD_TESTS "Build hike tests" OFF)
if(HIKE_BUILD_TESTS OR HIKE_BUILD_ALL)
    add_subdirectory(tests)
endif()
