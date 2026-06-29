option(UFOEXECUTION_BUILD_DOCS "Generate documentation" OFF)
option(UFOEXECUTION_BUILD_TESTS "Unit testing" OFF)
option(UFOEXECUTION_BUILD_COVERAGE "Test Coverage" OFF)

add_library(Execution INTERFACE)
add_library(UFO::Execution ALIAS Execution)

target_link_libraries(Execution INTERFACE UFO::Utility)

# Threading library required in Linux (not in MacOS and Windows, they should just ignore it. Windows might give a warning)
find_package(Threads REQUIRED)
target_link_libraries(Execution INTERFACE Threads::Threads)

include(CheckCXXSourceCompiles)
include(CMakePushCheckState)

message(CHECK_START "Finding parallel processing libraries")
list(APPEND CMAKE_MESSAGE_INDENT "  ")
unset(missingParallelLibs)

message(CHECK_START "Finding Threading Building Blocks (TBB)")

# include("${PROJECT_SOURCE_DIR}/3rdparty/tbb/tbb.cmake")
find_package(TBB QUIET)

if(TBB_FOUND)
	list(APPEND CMAKE_MESSAGE_INDENT "  ")
	message(CHECK_START "Checking if oneTBB and not TBB")

	cmake_push_check_state()
	set(CMAKE_REQUIRED_FLAGS "-O0")
	set(CMAKE_REQUIRED_LIBRARIES "TBB::tbb")

	check_cxx_source_compiles("
			// STL
			#include <cstddef>
			#include <vector>

			// oneTBB
			#include <oneapi/tbb.h>
			
			struct S {
				std::size_t value;
			};

			int main() {
				std::vector<S> v(10);

				oneapi::tbb::parallel_for(std::size_t(0), v.size(), [&v](std::size_t i) { v[i].value = i; });

				return 0;
			}
		" HAVE_ONETBB_PAR)

	cmake_pop_check_state()

	if(HAVE_ONETBB_PAR)
		message(CHECK_PASS "has oneTBB")
		list(POP_BACK CMAKE_MESSAGE_INDENT)
		message(CHECK_PASS "found and enabled")
		target_link_libraries(Execution INTERFACE TBB::tbb)
		target_compile_definitions(Execution INTERFACE UFO_PAR_TBB=1)

	else()
		message(CHECK_FAIL "only has old TBB (not oneTBB)")
		list(POP_BACK CMAKE_MESSAGE_INDENT)
		message(CHECK_FAIL "not found and disabled")
		list(APPEND missingParallelLibs TBB)
	endif()

else()
	message(CHECK_FAIL "not found and disabled")
	list(APPEND missingParallelLibs TBB)
endif()

message(CHECK_START "Checking Standard Template Library (STL) parallel support")

cmake_push_check_state()

set(CMAKE_REQUIRED_FLAGS "-O0")
check_cxx_source_compiles("
		#include <algorithm>
		#include <array>
		#include <execution>
		int main() {
			std::array<int, 10> a{};
			std::for_each(std::execution::par, a.begin(), a.end(), [](auto){});
			(void)a;
			return 0;
		}
	" HAVE_STL_PAR)

if(HAVE_STL_PAR)
	message(CHECK_PASS "has native support")
	target_compile_definitions(Execution INTERFACE UFO_PAR_STL=1)

else()
	if(TBB_FOUND)
		set(CMAKE_REQUIRED_LIBRARIES "TBB::tbb")
		check_cxx_source_compiles("
				#include <algorithm>
				#include <array>
				#include <execution>
				int main() {
					std::array<int, 10> a{};
					std::for_each(std::execution::par, a.begin(), a.end(), [](auto){});
					(void)a;
					return 0;
				}
			" HAVE_STL_PAR_USING_TBB)

		if(HAVE_STL_PAR_USING_TBB)
			message(CHECK_PASS "has support using TBB")
			target_link_libraries(Execution INTERFACE TBB::tbb)
			target_compile_definitions(Execution INTERFACE UFO_PAR_STL=1)
		else()
			message(CHECK_FAIL "no support")
			list(APPEND missingParallelLibs STL)
		endif()

	else()
		message(CHECK_FAIL "no support")
		list(APPEND missingParallelLibs STL)
	endif()
endif()

cmake_pop_check_state()

message(CHECK_START "Finding Grand Central Dispatch (GCD)")

if(APPLE)
	message(CHECK_PASS "found and enabled")
	target_compile_definitions(Execution INTERFACE UFO_PAR_GCD=1)
else()
	message(CHECK_FAIL "not found and disabled")
	list(APPEND missingParallelLibs GCD)
endif()

message(CHECK_START "Finding OpenMP (OMP)")
find_package(OpenMP QUIET)

if(OpenMP_CXX_FOUND)
	message(CHECK_PASS "found and enabled")
	target_link_libraries(Execution INTERFACE OpenMP::OpenMP_CXX)
	target_compile_definitions(Execution INTERFACE UFO_PAR_OMP=1)
else()
	message(CHECK_FAIL "not found and disabled")
	list(APPEND missingParallelLibs OMP)
endif()

list(POP_BACK CMAKE_MESSAGE_INDENT)

if(missingParallelLibs)
	message(CHECK_FAIL "disabled parallel processing libraries: ${missingParallelLibs}")
else()
	message(CHECK_PASS "all parallel processing libraries found and enabled")
endif()

include(GNUInstallDirs)

target_include_directories(Execution
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

if(UFO_BUILD_TESTS OR UFOEXECUTION_BUILD_TESTS)
	add_subdirectory(tests)
endif()

if(UFO_BUILD_DOCS OR UFOEXECUTION_BUILD_DOCS)
	add_subdirectory(docs)
endif()

install(TARGETS Execution EXPORT Execution-targets
	COMPONENT Execution
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

install(EXPORT Execution-targets
	FILE "Execution-targets.cmake"
	NAMESPACE UFO::
	DESTINATION lib/cmake/${PROJECT_NAME}
	COMPONENT Execution
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/Execution-config-version.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/Execution-config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/Execution-config.cmake"
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
	FILES
	"${CMAKE_CURRENT_BINARY_DIR}/Execution-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/Execution-config-version.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
	COMPONENT Execution
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
	COMPONENT Execution
	DESTINATION ${CMAKE_INSTALL_PREFIX}
)