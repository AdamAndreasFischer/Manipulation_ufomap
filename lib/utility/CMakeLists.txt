if(${UFO_SUPER_BUILD})
	include(super.cmake)
else()
	cmake_minimum_required(VERSION 3.16...3.24)

	if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
		set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
		set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
	endif()

	project(ufoutility
		VERSION 1.0.0
		DESCRIPTION "The UFO utility library"
		LANGUAGES CXX
	)

	add_subdirectory(3rdparty)

	add_library(Utility SHARED 
		src/io/read_buffer.cpp
		src/io/write_buffer.cpp
		src/io/buffer.cpp
	)
	add_library(UFO::Utility ALIAS Utility)

	# include("${PROJECT_SOURCE_DIR}/3rdparty/tbb/tbb.cmake")
	find_package(TBB REQUIRED)

	# find_package(OpenMP)
	# if(OpenMP_CXX_FOUND)
	#     target_link_libraries(Utility PUBLIC OpenMP::OpenMP_CXX)
	# 		target_compile_definitions(Utility
	# 			PUBLIC
	# 				UFO_OMP=1
	# 		)
	# endif()

	target_link_libraries(Utility PUBLIC TBB::tbb)

	target_compile_definitions(Utility
		PUBLIC
			UFO_TBB=1
	)

	set_target_properties(Utility PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
	)

	include(GNUInstallDirs)

	target_include_directories(Utility 
		PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
			$<INSTALL_INTERFACE:include>
	)

	install(TARGETS Utility EXPORT ufoutility-targets
		LIBRARY  DESTINATION lib
		ARCHIVE  DESTINATION lib
		RUNTIME  DESTINATION bin
		INCLUDES DESTINATION include
	)

	install(EXPORT ufoutility-targets
		FILE "ufoutility-targets.cmake"
		NAMESPACE UFO::
		DESTINATION lib/cmake/${PROJECT_NAME}
	)

	include(CMakePackageConfigHelpers)
	write_basic_package_version_file(
		"${CMAKE_CURRENT_BINARY_DIR}/ufoutility-config-version.cmake"
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY SameMajorVersion
	)

	configure_package_config_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/ufoutility-config.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/ufoutility-config.cmake"
		INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
	)

	install(
		FILES
			"${CMAKE_CURRENT_BINARY_DIR}/ufoutility-config.cmake"
			"${CMAKE_CURRENT_BINARY_DIR}/ufoutility-config-version.cmake"
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
	)

	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
		DESTINATION ${CMAKE_INSTALL_PREFIX}
	)
endif()