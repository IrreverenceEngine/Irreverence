cmake_minimum_required(VERSION 3.6)
project(irreverence LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/game/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/game/bin)

add_subdirectory(vendor/glm)

set(BGFX_BUILD_EXAMPLES OFF)
set(BGFX_BUILD_TOOLS OFF)
add_subdirectory(vendor/bgfx.cmake)

set(SDL_EXAMPLES OFF)
add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)

file(GLOB_RECURSE IRREVERENCE_SRC CONFIGURE_DEPENDS src/*.cpp)

add_executable(irreverence ${IRREVERENCE_SRC})

target_link_libraries(irreverence PRIVATE
	SDL3-shared glm bgfx
)

target_include_directories(irreverence PUBLIC
    src/
    vendor/glm/glm
    vendor/bgfx.cmake/bgfx/include
    vendor/SDL/include
)
