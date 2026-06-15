cmake_minimum_required(VERSION 3.14)
project(SimplyChess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

# Gather source files
file(GLOB SOURCES "src/*.cpp")

# Include headers
include_directories(include)

# Add compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# Create executable
add_executable(SimplyChess ${SOURCES})

# Link SFML 2.6
target_link_libraries(SimplyChess PRIVATE sfml-graphics sfml-window sfml-system)

# Copiar recursos al directorio de construcción
add_custom_command(TARGET SimplyChess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources ${CMAKE_BINARY_DIR}/Resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Config ${CMAKE_BINARY_DIR}/Config
)
