cmake_minimum_required(VERSION 3.16...3.24)

# The default build type must be set before project()
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

project(UFO
	VERSION 1.0.0
	DESCRIPTION "UFO"
	LANGUAGES CXX C
)

option(UFO_BUILD_DOCS     "Generate documentation"             OFF)
option(UFO_BUILD_TESTS  "Unit testing"                       OFF)
option(UFO_COVERAGE "Test Coverage"                      OFF)
option(UFO_DEV_MODE "Set up development helper settings" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_CLANG_TIDY clang-tidy)

set(UFO_SUPER_BUILD ON)

add_subdirectory(lib/cloud)
add_subdirectory(lib/compute)
add_subdirectory(lib/container)
add_subdirectory(lib/core)
add_subdirectory(lib/execution)
add_subdirectory(lib/geometry)
add_subdirectory(lib/map)
add_subdirectory(lib/math)
add_subdirectory(lib/morton)
add_subdirectory(lib/plan)
add_subdirectory(lib/time)
add_subdirectory(lib/utility)
add_subdirectory(lib/vision)
# add_subdirectory(lib/viz)

include(GNUInstallDirs)

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/ufo-config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/ufo-config.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/ufo-config-version.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

INSTALL(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/ufo-config.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/ufo-config-version.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)