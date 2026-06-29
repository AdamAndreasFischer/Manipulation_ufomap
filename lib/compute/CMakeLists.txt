option(UFOCOMPUTE_BUILD_DOCS "Generate documentation" OFF)
option(UFOCOMPUTE_BUILD_TESTS "Unit testing" OFF)
option(UFOCOMPUTE_BUILD_COVERAGE "Test Coverage" OFF)

include(GNUInstallDirs)

# TODO: Make option to make shared or static
add_library(Compute SHARED
	src/compute.cpp
)
add_library(UFO::Compute ALIAS Compute)

set_target_properties(Compute
	PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
		OUTPUT_NAME "UFOCompute"
)

set(WGPU "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/wgpu")
set(WGPU_RUNTIME_LIB "${WGPU}/lib/libwgpu_native.so")

if (WIN32)
	set(OS_LIBRARIES d3dcompiler ws2_32 userenv bcrypt ntdll opengl32 Propsys RuntimeObject)
elseif(UNIX AND NOT APPLE)
	set(OS_LIBRARIES "-lm -ldl")
elseif(APPLE)
	set(OS_LIBRARIES "-framework Foundation -framework CoreFoundation -framework QuartzCore -framework Metal")
endif()

target_link_libraries(Compute
	PUBLIC
		$<BUILD_INTERFACE:${WGPU_RUNTIME_LIB}>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/lib/libwgpu_native.so>
		${OS_LIBRARIES}
)

target_include_directories(Compute
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<BUILD_INTERFACE:${WGPU}/include>
		$<INSTALL_INTERFACE:include>
)

if(UFO_BUILD_TESTS OR UFOCOMPUTE_BUILD_TESTS)
	add_subdirectory(tests)
endif()

if(UFO_BUILD_DOCS OR UFOCOMPUTE_BUILD_DOCS)
	add_subdirectory(docs)
endif()

install(TARGETS Compute EXPORT Compute-targets
	COMPONENT Compute
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

install(EXPORT Compute-targets
	FILE "Compute-targets.cmake"
	NAMESPACE UFO::
	DESTINATION lib/cmake/${PROJECT_NAME}
	COMPONENT Compute
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/Compute-config-version.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/Compute-config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/Compute-config.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
	FILES
	"${CMAKE_CURRENT_BINARY_DIR}/Compute-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/Compute-config-version.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
	COMPONENT Compute
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
	COMPONENT Compute
	DESTINATION ${CMAKE_INSTALL_PREFIX}
)

install(FILES ${WGPU}/lib/libwgpu_native.so 
	COMPONENT Compute
	DESTINATION lib
)
install(DIRECTORY ${WGPU}/include
	COMPONENT Compute
	DESTINATION ${CMAKE_INSTALL_PREFIX}
)