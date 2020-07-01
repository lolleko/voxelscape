cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if (UNIX)
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()
if ( MSVC )
  #set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /Wall /WX")
  set(CMAKE_CXX_FLAGS_RELEASE "/O3 /MD")
endif()

if(DEFINED ENV{VCPKG_INSTALLATION_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

project(voxelscape
  LANGUAGES CXX
)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_executable("${CMAKE_PROJECT_NAME}" ${sources})

target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES 
  CXX_STANDARD 17 
  OUTPUT_NAME "${CMAKE_PROJECT_NAME}"
)

find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE ${OpenMP_CXX_LIBRARIES})
endif()

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

find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE nlohmann_json::nlohmann_json)

find_package(EnTT CONFIG REQUIRED)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE EnTT::EnTT)

find_package(unofficial-concurrentqueue CONFIG REQUIRED)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE unofficial::concurrentqueue::concurrentqueue)

# Resources
add_custom_command(TARGET "${CMAKE_PROJECT_NAME}" PRE_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/resources/ ${CMAKE_BINARY_DIR}/resources)

install(TARGETS "${CMAKE_PROJECT_NAME}" RUNTIME DESTINATION . COMPONENT App)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources DESTINATION . COMPONENT Resources)

set(CPACK_PACKAGE_DESCRIPTION "Just another CPacke example")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_BUNDLE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_BUNDLE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_GENERATOR "ZIP")

include (CPack)