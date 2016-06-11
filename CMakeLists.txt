cmake_minimum_required(VERSION 2.8)

project(particles)

if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/ext/sfml")
  message(FATAL_ERROR "Missing SFML repository!"
  	"Particles depends on the SFML library and has to be cloned with the --recursive flag."
    "Fix by calling \"git submodule update --init --recursive\"")
endif()

option(PARTICLES_BUILD_DEMO "Build demo application?" OFF)

if(WIN32)
	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
	CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
	if(COMPILER_SUPPORTS_CXX11)
	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	elseif(COMPILER_SUPPORTS_CXX0X)
	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	else()
	        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
	endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++11")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
endif()

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

add_subdirectory("${PROJECT_SOURCE_DIR}/ext/sfml")

include_directories(
	"${PROJECT_SOURCE_DIR}/ext/sfml/include"
	"${PROJECT_SOURCE_DIR}"
)

add_library(particles STATIC
	"${PROJECT_SOURCE_DIR}/Particles/ParticleData.cpp"
	"${PROJECT_SOURCE_DIR}/Particles/ParticleGenerator.cpp"
	"${PROJECT_SOURCE_DIR}/Particles/ParticleSystem.cpp"
	"${PROJECT_SOURCE_DIR}/Particles/ParticleUpdater.cpp"
)

target_link_libraries(particles sfml-system sfml-graphics)

if (PARTICLES_BUILD_DEMO)
	find_package(AntTweakBar REQUIRED)
	include_directories("${ANT_TWEAK_BAR_INCLUDE_PATH}" "${PROJECT_SOURCE_DIR}/demo")
	add_executable(demo "${PROJECT_SOURCE_DIR}/demo/main.cpp" "${PROJECT_SOURCE_DIR}/demo/AntTweakBarInput.cpp")
	target_link_libraries(demo ${ANT_TWEAK_BAR_LIBRARY} sfml-window particles)

	file(COPY demo/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
