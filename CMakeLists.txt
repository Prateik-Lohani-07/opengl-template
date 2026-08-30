cmake_minimum_required(VERSION 3.20)

project(OpenGLTemplate)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
	src/main.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# preprocessing directives
target_compile_definitions(${PROJECT_NAME} PRIVATE
	GLEW_STATIC
)

# setting compiler to MSVC 
target_compile_options(${PROJECT_NAME} PRIVATE
	/W4
)

# setting include header file paths
target_include_directories(${PROJECT_NAME} PRIVATE
	dependencies/glfw/include
	dependencies/glew/include
)

target_link_directories(${PROJECT_NAME} PRIVATE
	dependencies/glfw/lib-vc2026/
	dependencies/glew/lib/Release/Win32
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	glfw3
	opengl32
	glew32s
)