option(UFOGEOMETRY_BUILD_DOCS     "Generate documentation" OFF)
option(UFOGEOMETRY_BUILD_TESTS    "Unit testing"           OFF)
option(UFOGEOMETRY_BUILD_COVERAGE "Test Coverage"          OFF)

add_library(Geometry INTERFACE)
add_library(UFO::Geometry ALIAS Geometry)

target_link_libraries(Geometry 
	INTERFACE 
		UFO::Math
)

include(GNUInstallDirs)

target_include_directories(Geometry 
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

if(UFO_BUILD_TESTS OR UFOGEOMETRY_BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(UFO_BUILD_DOCS OR UFOGEOMETRY_BUILD_DOCS)
	add_subdirectory(docs)
endif()

install(TARGETS Geometry EXPORT Geometry-targets
	COMPONENT Geometry
	LIBRARY  DESTINATION lib
	ARCHIVE  DESTINATION lib
	RUNTIME  DESTINATION bin
  INCLUDES DESTINATION include
)

install(EXPORT Geometry-targets
  FILE "Geometry-targets.cmake"
  NAMESPACE UFO::
  DESTINATION lib/cmake/${PROJECT_NAME}
	COMPONENT Geometry
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/Geometry-config-version.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/Geometry-config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/Geometry-config.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/Geometry-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/Geometry-config-version.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  COMPONENT Geometry
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
	COMPONENT Geometry
	DESTINATION ${CMAKE_INSTALL_PREFIX}
)