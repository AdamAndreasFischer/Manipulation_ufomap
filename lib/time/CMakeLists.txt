option(UFOTIME_BUILD_DOCS     "Generate documentation" OFF)
option(UFOTIME_BUILD_TESTS    "Unit testing"           OFF)
option(UFOTIME_BUILD_COVERAGE "Test Coverage"          OFF)

add_library(Time SHARED 
	src/timer.cpp
	src/timing.cpp
)
add_library(UFO::Time ALIAS Time)

set_target_properties(Time 
	PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
		OUTPUT_NAME "UFOTime"
)

include(GNUInstallDirs)

target_include_directories(Time 
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

if(UFO_BUILD_TESTS OR UFOTIME_BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(UFO_BUILD_DOCS OR UFOTIME_BUILD_DOCS)
	add_subdirectory(docs)
endif()

install(TARGETS Time EXPORT Time-targets
	COMPONENT Time
	LIBRARY  DESTINATION lib
	ARCHIVE  DESTINATION lib
	RUNTIME  DESTINATION bin
  INCLUDES DESTINATION include
)

install(EXPORT Time-targets
  FILE "Time-targets.cmake"
  NAMESPACE UFO::
  DESTINATION lib/cmake/${PROJECT_NAME}
	COMPONENT Time
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/Time-config-version.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/Time-config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/Time-config.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/Time-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/Time-config-version.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  COMPONENT Time
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
	COMPONENT Time
	DESTINATION ${CMAKE_INSTALL_PREFIX}
)