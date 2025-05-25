cmake_minimum_required(VERSION 3.28)
project(LordsAndCodes LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SFML_STATIC_LIBRARIES FALSE CACHE BOOL "Use static SFML libraries")
set(BUILD_SHARED_LIBS TRUE CACHE BOOL "Build shared libraries")

remove_definitions(-DSFML_STATIC)

set(SFML_DIR "D:/libs/SFML-3.0.0/lib/cmake/SFML")
find_package(SFML REQUIRED COMPONENTS Graphics Window System)


file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${PROJECT_NAME} ${GAME_SRC})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        SFML::Graphics      
)

if (WIN32 AND NOT SFML_STATIC_LIBRARIES)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_DIR}/../../../bin/sfml-graphics-d-3.dll"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_DIR}/../../../bin/sfml-window-d-3.dll"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_DIR}/../../../bin/sfml-system-d-3.dll"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
    )
endif()
