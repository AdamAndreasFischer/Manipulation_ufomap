option(UFOCONTAINER_BUILD_DOCS     "Generate documentation" OFF)
option(UFOCONTAINER_BUILD_TESTS    "Unit testing"           OFF)
option(UFOCONTAINER_BUILD_COVERAGE "Test Coverage"          OFF)

add_library(Container INTERFACE)
add_library(UFO::Container ALIAS Container)

target_link_libraries(Container 
	INTERFACE
		UFO::Compute
		UFO::Execution
		UFO::Geometry 
		UFO::Morton 
		UFO::Utility
		UFO::Vision
)

include(GNUInstallDirs)

target_include_directories(Container 
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

if(UFO_BUILD_TESTS OR UFOCONTAINER_BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(UFO_BUILD_DOCS OR UFOCONTAINER_BUILD_DOCS)
	add_subdirectory(docs)
endif()

install(TARGETS Container EXPORT Container-targets
	COMPONENT Container
	LIBRARY  DESTINATION lib
	ARCHIVE  DESTINATION lib
	RUNTIME  DESTINATION bin
  INCLUDES DESTINATION include
)

install(EXPORT Container-targets
  FILE "Container-targets.cmake"
  NAMESPACE UFO::
  DESTINATION lib/cmake/${PROJECT_NAME}
	COMPONENT Container
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/Container-config-version.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/Container-config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/Container-config.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/Container-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/Container-config-version.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  COMPONENT Container
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
	COMPONENT Container
	DESTINATION ${CMAKE_INSTALL_PREFIX}
)