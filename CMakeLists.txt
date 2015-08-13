cmake_minimum_required(VERSION 2.6)

project(particles)

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