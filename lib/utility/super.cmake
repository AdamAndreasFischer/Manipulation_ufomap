option(UFOUTILITY_BUILD_DOCS     "Generate documentation" OFF)
option(UFOUTILITY_BUILD_TESTS    "Unit testing"           OFF)
option(UFOUTILITY_BUILD_COVERAGE "Test Coverage"          OFF)

add_library(Utility SHARED 
	src/io/read_buffer.cpp
	src/io/write_buffer.cpp
	src/io/buffer.cpp
)
add_library(UFO::Utility ALIAS Utility)

set_target_properties(Utility 
	PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
		OUTPUT_NAME "UFOUtility"
)

include(GNUInstallDirs)

target_include_directories(Utility 
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

install(TARGETS Utility EXPORT Utility-targets
	COMPONENT Utility
	LIBRARY  DESTINATION lib
	ARCHIVE  DESTINATION lib
	RUNTIME  DESTINATION bin
  INCLUDES DESTINATION include
)

install(EXPORT Utility-targets
  FILE "Utility-targets.cmake"
  NAMESPACE UFO::
  DESTINATION lib/cmake/${PROJECT_NAME}
	COMPONENT Utility
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/Utility-config-version.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/Utility-config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/Utility-config.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/Utility-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/Utility-config-version.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  COMPONENT Utility
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
	COMPONENT Utility
	DESTINATION ${CMAKE_INSTALL_PREFIX}
)