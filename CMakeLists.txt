cmake_minimum_required(VERSION 3.22)
project(irreverence LANGUAGES CXX C)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/game)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/game)

add_subdirectory(vendor/IRX)

option(ENABLE_TRACY "Enable Tracy profiling" OFF)

find_package(OpenGL REQUIRED)

# NOTE: Copy pasted from older project, if not work seek help.
file(DOWNLOAD https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz ${CMAKE_BINARY_DIR}/vendor/glew.tgz)
file(ARCHIVE_EXTRACT INPUT ${CMAKE_BINARY_DIR}/vendor/glew.tgz DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glew)

set(BUILD_UTILS OFF)
add_subdirectory(vendor/glew/glew-2.2.0/build/cmake)

set(SDL_EXAMPLES OFF)
add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)

set(CROSS_PLATFORM_DETERMINISTIC ON)
set(DEBUG_RENDERER_IN_DEBUG_AND_RELEASE OFF)
add_subdirectory(vendor/JoltPhysics/Build)

if (WIN32)
	set(ALSOFT_UTILS OFF)
	add_subdirectory(vendor/openal-soft)
else()
	find_package(OpenAL REQUIRED)
endif()

set(RECASTNAVIGATION_DEMO OFF)
set(RECASTNAVIGATION_TESTS OFF)
set(RECASTNAVIGATION_EXAMPLES OFF)
add_subdirectory(vendor/recastnavigation)

file(GLOB_RECURSE IRREVERENCE_SRC CONFIGURE_DEPENDS src/*.cpp)

add_executable(irreverence ${IRREVERENCE_SRC})

if (ENABLE_TRACY)
	target_compile_definitions(irreverence PUBLIC -DIR_USE_TRACY)
endif()

# if on Windows alias openal to OpenAL
if (WIN32)
    add_library(openal INTERFACE IMPORTED)
    set_target_properties(openal PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/vendor/openal-soft/include"
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/vendor/openal-soft/libopenal.so"
    )
endif()

add_subdirectory(vendor/tracy)

target_link_libraries(irreverence PRIVATE
    glm::glm SDL3-static glew_s OpenGL::GL Jolt
  	openal Tracy::TracyClient
    RecastNavigation::Recast RecastNavigation::Detour RecastNavigation::DetourTileCache
    irx
)

target_include_directories(irreverence PUBLIC
    src/
  	vendor/glew/glew-2.2.0/include
    vendor/SDL/include
    vendor/JoltPhysics/
  	vendor/openal-soft/include
  	vendor/tracy/public
    vendor/recastnavigation/Recast/Include
    vendor/recastnavigation/Detour/Include
    vendor/recastnavigation/DetourTileCache/Include
    irx
)
