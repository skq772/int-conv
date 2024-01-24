cmake_minimum_required(VERSION 3.20)
project(int-conv)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

find_package(OpenGL REQUIRED)

set(IMGUI_SOURCES
	imgui/imgui.cpp imgui/imgui.h
	imgui/imconfig.h
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/imgui_internal.h
	imgui/imstb_rectpack.h
	imgui/imstb_textedit.h
	imgui/imstb_truetype.h
	imgui/backends/imgui_impl_glfw.h
	imgui/backends/imgui_impl_glfw.cpp
	imgui/backends/imgui_impl_opengl3.h
	imgui/backends/imgui_impl_opengl3.cpp
	imgui/misc/cpp/imgui_stdlib.h
	imgui/misc/cpp/imgui_stdlib.cpp
)

set(CURRENT_PROJECT_SOURCES
	src/main.cpp
	src/stdvar.h
	src/convert.h
	src/convert.c
)

include_directories(
	imgui
	imgui/misc/cpp
	imgui/backends
)

add_executable(${PROJECT_NAME} ${CURRENT_PROJECT_SOURCES} ${IMGUI_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "imgui")
target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw)
