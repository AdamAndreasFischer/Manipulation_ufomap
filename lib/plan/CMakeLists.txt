option(UFOPLAN_BUILD_DOCS     "Generate documentation" OFF)
option(UFOPLAN_BUILD_TESTS    "Unit testing"           OFF)
option(UFOPLAN_BUILD_COVERAGE "Test Coverage"          OFF)

add_library(Plan INTERFACE)
add_library(UFO::Plan ALIAS Plan)

target_link_libraries(Plan 
	INTERFACE
		UFO::Container
		UFO::Map
		UFO::Math
)

include(GNUInstallDirs)

target_include_directories(Plan 
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

if(UFO_BUILD_TESTS OR UFOPLAN_BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(UFO_BUILD_DOCS OR UFOPLAN_BUILD_DOCS)
	add_subdirectory(docs)
endif()

install(TARGETS Plan EXPORT Plan-targets
	COMPONENT Plan
	LIBRARY  DESTINATION lib
	ARCHIVE  DESTINATION lib
	RUNTIME  DESTINATION bin
  INCLUDES DESTINATION include
)

install(EXPORT Plan-targets
  FILE "Plan-targets.cmake"
  NAMESPACE UFO::
  DESTINATION lib/cmake/${PROJECT_NAME}
	COMPONENT Plan
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/Plan-config-version.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/Plan-config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/Plan-config.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/Plan-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/Plan-config-version.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  COMPONENT Plan
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
	COMPONENT Plan
	DESTINATION ${CMAKE_INSTALL_PREFIX}
)