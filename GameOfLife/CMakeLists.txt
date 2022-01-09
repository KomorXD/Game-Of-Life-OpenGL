# CMakeList.txt : CMake project for CMakeOpenGL, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)

include_directories(
	../dependencies/glfw/include/
	../dependencies/glad/include/
	../dependencies/glm/
)

file(GLOB PROJECT_HEADERS *.hpp)
file(GLOB PROJECT_SOURCES *.cpp)
file(GLOB VENDORS_SOURCES ../dependencies/glad/src/glad.c)
file(GLOB PROJECT_CONFIGS
	../CMakeLists.txt
	../.gitignore
	../.gitmodules
)
file(COPY res DESTINATION "${CMAKE_BINARY_DIR}/${PROJECT_NAME}")

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${PROJECT_CONFIGS} ${VENDORS_SOURCES})

target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} ${GLAD_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

# TODO: Add tests and install targets if needed.
