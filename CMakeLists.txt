cmake_minimum_required(VERSION 2.6)

project(particles)



set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

find_package(SFML 2 COMPONENTS system window graphics audio REQUIRED)

include_directories(
	"${SFML_INCLUDE_DIR}"
	"${PROJECT_SOURCE_DIR}/src"
)

file(GLOB_RECURSE particles_FILES
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(particles ${particles_FILES})

target_link_libraries(particles ${SFML_LIBRARIES})