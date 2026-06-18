cmake_minimum_required(VERSION 3.14)
project(SimplyChess VERSION 1.0.0 LANGUAGES CXX)

# Estándar C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# SFML 2.6.1
include(FetchContent)
FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

# Añadir source files de src/ y sus subcarpetas (core, states, ui, chess).
# CONFIGURE_DEPENDS re-detecta nuevos .cpp al reconfigurar.
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# Incluir headers
include_directories(include)

# Añadir warnings del compilador (los flags GNU no son válidos en MSVC)
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Crear ejecutable
add_executable(SimplyChess ${SOURCES})

# Enlazar SFML 2.6.1
target_link_libraries(SimplyChess PRIVATE sfml-graphics sfml-window sfml-system)

# Copiar recursos al directorio de construcción
add_custom_command(TARGET SimplyChess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/config ${CMAKE_BINARY_DIR}/config
)
