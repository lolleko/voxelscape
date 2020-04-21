cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

if(DEFINED ENV{VCPKG_INSTALLATION_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(voxelscape
  LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_executable("${CMAKE_PROJECT_NAME}" ${sources})

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

find_package(EnTT CONFIG REQUIRED)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE EnTT::EnTT)