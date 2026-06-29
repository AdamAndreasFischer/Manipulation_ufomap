include(FetchContent)

# set(OrbbecSDK_DIR "/usr/local/lib/") # Make this more generic
# find_package(OrbbecSDK QUIET)

# if(NOT OrbbecSDK_FOUND)
# 	string(TOLOWER "OrbbecSDK" FC_NAME)

# 	FetchContent_Declare(
# 		${FC_NAME}
# 		URL https://github.com/orbbec/OrbbecSDK_v2/releases/download/v2.4.8/OrbbecSDK_v2.4.8_202507031325_ec8e3469_linux_x86_64.zip
# 		URL_HASH SHA256=4a42a1bfd0032389f3be00b125166013abef5fbaa0a2297493d062caa9cf4d0f
# 	)

# 	FetchContent_MakeAvailable(OrbbecSDK)

# 	set(OrbbecSDK_DIR "${${FC_NAME}_SOURCE_DIR}")

# 	add_library(OrbbecSDK SHARED IMPORTED GLOBAL)
# 	add_library(ob::OrbbecSDK ALIAS OrbbecSDK)

# 	set_target_properties(OrbbecSDK
# 		PROPERTIES
# 			IMPORTED_LOCATION "${OrbbecSDK_DIR}/lib/libOrbbecSDK.so"
# 			IMPORTED_NO_SONAME TRUE
# 	)

# 	target_include_directories(OrbbecSDK
# 		INTERFACE
# 			$<BUILD_INTERFACE:${OrbbecSDK_DIR}/include>
# 			$<INSTALL_INTERFACE:include>
# 	)

# 	install(IMPORTED_RUNTIME_ARTIFACTS OrbbecSDK)

# 	install(DIRECTORY "${OrbbecSDK_DIR}/include"
# 		DESTINATION ${CMAKE_INSTALL_PREFIX}
# 	)
# endif()

FetchContent_Declare(
	tomlplusplus
	GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
	GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

find_package(Threads REQUIRED)

find_package(PCL 1.8 REQUIRED)

add_executable(ufo_manipulation ufo_manipulation.cpp)

set_target_properties(ufo_manipulation
	PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
		OUTPUT_NAME "UFOManipulation"
)

target_link_libraries(ufo_manipulation PRIVATE UFO::Map tomlplusplus::tomlplusplus Threads::Threads ${PCL_LIBRARIES}) # ob::OrbbecSDK)

install(TARGETS ufo_manipulation RUNTIME DESTINATION bin)