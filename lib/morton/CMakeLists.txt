option(UFOMORTON_BUILD_DOCS "Generate documentation" OFF)
option(UFOMORTON_BUILD_TESTS "Unit testing" OFF)
option(UFOMORTON_BUILD_COVERAGE "Test Coverage" OFF)

option(UFOMORTON_BMI2 "Enable BMI2 instruction set (if available)" OFF)

add_library(Morton INTERFACE)
add_library(UFO::Morton ALIAS Morton)

target_link_libraries(Morton INTERFACE UFO::Math)

if(UFOMORTON_BMI2)
	message(CHECK_START "Checking BMI2 instruction set support")

	include(CheckCXXSourceCompiles)
	include(CMakePushCheckState)

	cmake_push_check_state()

	set(TEST_FLAG "-mbmi2")
	set(CMAKE_REQUIRED_FLAGS "${TEST_FLAG} -O0")
	check_cxx_source_compiles("
			#include <immintrin.h>
			int main() {
				auto a = _pdep_u64(0, 0);
				(void)a;
				return 0;
			}
		" HAVE_BMI2)

	if(HAVE_BMI2)
		message(CHECK_PASS "supported and enabled")
		target_compile_definitions(Morton INTERFACE UFO_MORTON_BMI2=1)
		target_compile_options(Morton INTERFACE -mbmi2)
	else()
		message(CHECK_FAIL "not supported and disabled")
	endif()

	cmake_pop_check_state()
endif()

include(GNUInstallDirs)

target_include_directories(Morton
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

if(UFO_BUILD_TESTS OR UFOMORTON_BUILD_TESTS)
	add_subdirectory(tests)
endif()

if(UFO_BUILD_DOCS OR UFOMORTON_BUILD_DOCS)
	add_subdirectory(docs)
endif()

install(TARGETS Morton EXPORT Morton-targets
	COMPONENT Morton
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

install(EXPORT Morton-targets
	FILE "Morton-targets.cmake"
	NAMESPACE UFO::
	DESTINATION lib/cmake/${PROJECT_NAME}
	COMPONENT Morton
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/Morton-config-version.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/Morton-config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/Morton-config.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
	FILES
	"${CMAKE_CURRENT_BINARY_DIR}/Morton-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/Morton-config-version.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
	COMPONENT Morton
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
	COMPONENT Morton
	DESTINATION ${CMAKE_INSTALL_PREFIX}
)