cmake_minimum_required(VERSION 3.6)
project(irreverence LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/game)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/game)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

add_subdirectory(vendor/glm)

set(SDL_EXAMPLES OFF)
add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)

file(GLOB_RECURSE IRREVERENCE_SRC CONFIGURE_DEPENDS src/*.cpp)

add_executable(irreverence ${IRREVERENCE_SRC})

target_link_libraries(irreverence PRIVATE
    glm::glm SDL3-static GLEW::GLEW OpenGL::GL
)

target_include_directories(irreverence PUBLIC
    src/
    vendor/glm/glm
    vendor/SDL/include
)
