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

if (WIN32)
    list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/resources/SimplyChess.rc")
endif()

# Crear ejecutable
if (APPLE)
    add_executable(SimplyChess MACOSX_BUNDLE ${SOURCES})
    set_target_properties(SimplyChess PROPERTIES 
        BUILD_WITH_INSTALL_RPATH ON 
        INSTALL_RPATH "@executable_path/../Frameworks"
        MACOSX_BUNDLE_ICON_FILE PeonN.icns
        MACOSX_BUNDLE_BUNDLE_NAME "Simply Chess"
    )
    set_source_files_properties(${CMAKE_SOURCE_DIR}/resources/images/icons/PeonN.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    target_sources(SimplyChess PRIVATE ${CMAKE_SOURCE_DIR}/resources/images/icons/PeonN.icns)
else()
    add_executable(SimplyChess ${SOURCES})
endif()

# Enlazar SFML 2.6.1
target_link_libraries(SimplyChess PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

# Copiar recursos al directorio de construcción o al bundle en macOS
if (APPLE)
    add_custom_command(TARGET SimplyChess POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:SimplyChess>/../Resources/resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/config $<TARGET_FILE_DIR:SimplyChess>/../Resources/config
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:SimplyChess>/../Frameworks
        COMMAND ditto
        ${CMAKE_BINARY_DIR}/_deps/sfml-src/extlibs/libs-osx/Frameworks $<TARGET_FILE_DIR:SimplyChess>/../Frameworks
    )
else()
    add_custom_command(TARGET SimplyChess POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/config ${CMAKE_BINARY_DIR}/config
    )
endif()
