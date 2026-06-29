message(CHECK_START "Finding Catch2")
find_package(Catch2 3 QUIET)
if(Catch2_FOUND)
	message(CHECK_PASS "found, it is installed on the system")
else()
	message(CHECK_FAIL "not found, will fetch it instead")
	
	Include(FetchContent)

	FetchContent_Declare(
	  Catch2
	  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	  GIT_TAG        8ac8190e494a381072c89f5e161b92a08d98b37b # v3.5.3
	  GIT_PROGRESS   TRUE
	)

	FetchContent_MakeAvailable(Catch2)
endif()

# SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
# SET(GCC_COVERAGE_LINK_FLAGS    "--coverage")
# SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
# SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
# # set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)

add_executable(ufocontainer_tests
	octree_test.cpp
	octree_map_test.cpp
	octree_set_test.cpp
	# quadtree_set_test.cpp
	# range_test.cpp
	# tree_index_test.cpp
	# tree_node_test.cpp
)

target_link_libraries(ufocontainer_tests PRIVATE UFO::Container Catch2::Catch2WithMain)

target_compile_features(ufocontainer_tests PRIVATE cxx_std_17)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(ufocontainer_tests)