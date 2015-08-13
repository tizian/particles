cmake_minimum_required(VERSION 2.6)

project(particles)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
<<<<<<< HEAD
=======
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
>>>>>>> afcc29554ee80f0dbb0be6b3ec46ee3d31665c85
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

find_package(SFML 2 COMPONENTS system window graphics audio REQUIRED)
find_package(AntTweakBar REQUIRED)

include_directories(
	"${SFML_INCLUDE_DIR}"
	"${ANT_TWEAK_BAR_INCLUDE_PATH}"
	"${PROJECT_SOURCE_DIR}/include"
)

file(GLOB_RECURSE particles_FILES
	"${PROJECT_SOURCE_DIR}/include/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(particles ${particles_FILES})

target_link_libraries(particles ${SFML_LIBRARIES} ${ANT_TWEAK_BAR_LIBRARY})