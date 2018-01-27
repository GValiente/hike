cmake_minimum_required(VERSION 3.5)
project(hike)

# Enable C++11:
set(CMAKE_CXX_STANDARD 11)

# Add a library with headers only:
add_library(${PROJECT_NAME} INTERFACE)

# Define include directories:
target_include_directories(${PROJECT_NAME}
    INTERFACE ${PROJECT_SOURCE_DIR}/include
)
