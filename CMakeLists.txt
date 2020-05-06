cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if(DEFINED ENV{VCPKG_INSTALLATION_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(voxelscape
  LANGUAGES CXX
)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_executable("${CMAKE_PROJECT_NAME}" ${sources} source/world/vs_chunk.cpp source/renderer/vs_mesh.cpp source/renderer/vs_model.cpp)

target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES 
  CXX_STANDARD 17 
  OUTPUT_NAME "${CMAKE_PROJECT_NAME}"
)

find_package(glad REQUIRED)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glad::glad)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glm)

find_package(imgui CONFIG REQUIRED)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE imgui::imgui)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE spdlog::spdlog spdlog::spdlog_header_only)

# https://github.com/microsoft/vcpkg/issues/9918
# find_package(assimp CONFIG REQUIRED)
# target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE assimp::assimp)

find_package(assimp CONFIG REQUIRED)

if(UNIX) 
  find_library(ASSIMP_ZLIB_LIBRARY z)
  find_library(ASSIMP_IRRXML_LIBRARY IrrXML)
  target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE 
    assimp::assimp
    ${ASSIMP_ZLIB_LIBRARY}
    ${ASSIMP_IRRXML_LIBRARY}
  )
else()
  target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE 
    assimp::assimp
  )
endif()



find_package(Stb REQUIRED)
target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE ${Stb_INCLUDE_DIR})

# Resources
add_custom_command(TARGET "${CMAKE_PROJECT_NAME}" PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders/ ${CMAKE_BINARY_DIR}/shaders)

add_custom_command(TARGET "${CMAKE_PROJECT_NAME}" PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/textures/ ${CMAKE_BINARY_DIR}/textures)

add_custom_command(TARGET "${CMAKE_PROJECT_NAME}" PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/models/ ${CMAKE_BINARY_DIR}/models)