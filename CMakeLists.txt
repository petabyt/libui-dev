# cmake -DCMAKE_TOOLCHAIN_FILE=extras/mingw.cmake -G Ninja -B build_win && cmake --build build_win
# cmake -G Ninja -B build && cmake --build build

# export LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib
# cmake -DCMAKE_TOOLCHAIN_FILE=extras/darwin.cmake -G Ninja -B build_osx && cmake --build build_osx

cmake_minimum_required(VERSION 3.20)
project(ui)

file(GLOB LIBUI_COMMON RELATIVE ${CMAKE_SOURCE_DIR} "common/*.c")
list(FILTER LIBUI_COMMON EXCLUDE REGEX ".*OLD_table\\.c$")

set(LIBUI_COMMON_FLAGS -fvisibility=hidden -fdiagnostics-color=always -D_FILE_OFFSET_BITS=64 -Wall -Winvalid-pch -Wextra -Wpedantic -Wno-unused-parameter -Wno-switch -D_UI_STATIC -Dlibui_EXPORTS)

if(WIN32)
	file(GLOB LIBUI_WIN RELATIVE ${CMAKE_SOURCE_DIR} "windows/*.cpp")

	add_library(ui_win32 STATIC ${LIBUI_COMMON} ${LIBUI_WIN}
		${CMAKE_CURRENT_SOURCE_DIR}/extras/favicon/win.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/extras/scroll.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/extras/win_init.cpp
	)
	target_include_directories(ui_win32 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/windows ${CMAKE_CURRENT_SOURCE_DIR} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
	target_compile_options(ui_win32
	    PRIVATE ${LIBUI_COMMON_FLAGS}
	)
elseif(APPLE)
	file(GLOB LIBUI_DARWIN RELATIVE ${CMAKE_SOURCE_DIR} "darwin/*.m")
	list(FILTER LIBUI_DARWIN EXCLUDE REGEX ".*OLD_table\\.m$")

	add_library(ui_darwin STATIC ${LIBUI_COMMON} ${LIBUI_DARWIN})
	target_include_directories(ui_darwin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/darwin ${CMAKE_CURRENT_SOURCE_DIR} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
	target_compile_options(ui_darwin
	    PRIVATE ${LIBUI_COMMON_FLAGS}
	)

	target_link_libraries(ui_darwin PUBLIC
	  "$<LINK_LIBRARY:FRAMEWORK,CoreFoundation,Foundation,AppKit,CoreText,CoreGraphics>"
	)
	
	add_library(LibUI::LibUI ALIAS ui_darwin)
elseif(UNIX)
	file(GLOB LIBUI_GTK3 RELATIVE ${CMAKE_SOURCE_DIR} "unix/*.c")
	list(FILTER LIBUI_GTK3 EXCLUDE REGEX ".*OLD_table\\.c$")

	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

	add_library(ui_gtk3 STATIC ${LIBUI_COMMON} ${LIBUI_GTK3}
		${CMAKE_CURRENT_SOURCE_DIR}/extras/favicon/linux.c
		${CMAKE_CURRENT_SOURCE_DIR}/extras/label.c
	)
	target_include_directories(ui_gtk3 PRIVATE ${GTK3_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/unix ${CMAKE_CURRENT_SOURCE_DIR} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
	target_link_libraries(ui_gtk3 ${GTK3_LIBRARIES})
	target_compile_options(ui_gtk3
	    PRIVATE ${LIBUI_COMMON_FLAGS}
	)
	add_library(LibUI::LibUI ALIAS ui_gtk3)
endif()

add_executable(hello-world examples/hello-world/main.c)
target_link_libraries(hello-world LibUI::LibUI)
