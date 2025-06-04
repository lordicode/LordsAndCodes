cmake_minimum_required(VERSION 3.28)
project(LordsAndCodes LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# SFML Configuration
set(SFML_STATIC_LIBRARIES FALSE CACHE BOOL "Use static SFML libraries")
set(BUILD_SHARED_LIBS TRUE CACHE BOOL "Build shared libraries")

# Remove SFML_STATIC definition if it exists
remove_definitions(-DSFML_STATIC)

# Try to find SFML, first with the specified path, then system-wide
set(SFML_DIR "D:/libs/SFML-3.0.0/lib/cmake/SFML" CACHE PATH "Path to SFML cmake config")
find_package(SFML 3.0 REQUIRED COMPONENTS Graphics Window System Audio)

# Collect source files - separate headers and sources
file(GLOB_RECURSE GAME_HEADERS CONFIGURE_DEPENDS "src/headers/*.h" "src/headers/*.hpp")
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# Combine all source files
set(GAME_SRC ${GAME_SOURCES} ${GAME_HEADERS})

# Debug: Print found files
message(STATUS "Found source files:")
foreach(source_file ${GAME_SOURCES})
    message(STATUS "  ${source_file}")
endforeach()
message(STATUS "Found header files:")
foreach(header_file ${GAME_HEADERS})
    message(STATUS "  ${header_file}")
endforeach()

# Create executable
add_executable(${PROJECT_NAME} ${GAME_SRC})

# Set C++ standard for the target
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/headers
)

# Link SFML libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
)

# Copy DLLs for Windows builds
if(WIN32 AND BUILD_SHARED_LIBS)
    # Set SFML bin directory
    set(SFML_BIN_DIR "${SFML_DIR}/../../../bin")
    
    # Copy SFML DLLs to output directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_BIN_DIR}/sfml-graphics$<$<CONFIG:Debug>:-d>-3.dll"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_BIN_DIR}/sfml-window$<$<CONFIG:Debug>:-d>-3.dll"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_BIN_DIR}/sfml-system$<$<CONFIG:Debug>:-d>-3.dll"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_BIN_DIR}/sfml-audio$<$<CONFIG:Debug>:-d>-3.dll"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/"
        COMMENT "Copying SFML DLLs to output directory"
    )
endif()

# Optional: Set working directory for Visual Studio
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
    )
endif()