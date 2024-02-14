cmake_minimum_required(VERSION 3.20)
project(int-conv)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
find_package(OpenGL REQUIRED)

add_subdirectory(freetype)

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
	imgui/misc/freetype/imgui_freetype.cpp
	imgui/misc/freetype/imgui_freetype.h
)

set(CURRENT_PROJECT_SOURCES
	src/main.cpp
	src/stdvar.h
	src/convert.h
	src/convert.cpp
)

include_directories(
	imgui
	imgui/misc/cpp
	imgui/misc/freetype
	imgui/backends
	${PROJECT_BINARY_DIR}
)

add_executable(bin2cc imgui/misc/fonts/binary_to_compressed_c.cpp)
add_custom_command(
	OUTPUT ${PROJECT_BINARY_DIR}/terminusttf.h
	DEPENDS bin2cc
	COMMAND bin2cc -base85 ../TerminusTTF-4.49.3.ttf FONT_TERMINUS > ${PROJECT_BINARY_DIR}/terminusttf.h
)

add_executable(${PROJECT_NAME} WIN32 ${CURRENT_PROJECT_SOURCES} ${PROJECT_BINARY_DIR}/terminusttf.h ${IMGUI_SOURCES})
target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_ENABLE_FREETYPE)

if(WIN32)
	# on linux this fails with an "attempted link of dynamic object [...] libGLX.so" ??
	# and on windows this makes it statically link with clang's libc++/libunwind
	target_link_libraries(${PROJECT_NAME} -static)
endif()
target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw freetype)
