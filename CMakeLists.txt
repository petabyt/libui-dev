# cmake -DCMAKE_TOOLCHAIN_FILE=extras/mingw.cmake -G Ninja -B build_win && cmake --build build_win
# cmake -G Ninja -B build && cmake --build build

# export LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib
# cmake -DCMAKE_TOOLCHAIN_FILE=extras/darwin.cmake -G Ninja -B build_osx && cmake --build build_osx

cmake_minimum_required(VERSION 3.15)
project(ui)

option(LIBUI_COMPILE_EXAMPLES "" ON)
option(LIBUI_INCLUDE_INSTALL "" ON)

file(GLOB LIBUI_COMMON ABSOLUTE ${PROJECT_SOURCE_DIR} "common/*.c")
list(FILTER LIBUI_COMMON EXCLUDE REGEX ".*OLD_table\\.c$")

set(LIBUI_COMMON_FLAGS -fvisibility=hidden -fdiagnostics-color=always -D_FILE_OFFSET_BITS=64 -Wall -Winvalid-pch -Wextra -Wpedantic -Wno-unused-parameter -Wno-switch -D_UI_STATIC -Dlibui_EXPORTS)

if(WIN32)
	file(GLOB LIBUI_WIN ABSOLUTE ${PROJECT_SOURCE_DIR} "windows/*.cpp")

	add_library(ui_win32 STATIC ${LIBUI_COMMON} ${LIBUI_WIN}
		${CMAKE_CURRENT_SOURCE_DIR}/extras/favicon/win.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/extras/scroll.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/extras/win_init.cpp
	)
	target_include_directories(ui_win32 PRIVATE ${PROJECT_SOURCE_DIR}/windows ${PROJECT_SOURCE_DIR} PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_compile_options(ui_win32
	    PRIVATE ${LIBUI_COMMON_FLAGS}
	)

	list(APPEND LIBS -luser32 -lkernel32 -lgdi32 -lcomctl32 -luxtheme -lmsimg32 -lcomdlg32 -ld2d1 -ldwrite -lole32 -loleaut32 -loleacc)
	list(APPEND LIBS -lstdc++ -lgcc -static -lpthread -lssp -lurlmon -luuid)
	list(APPEND LIBS -Wl,-subsystem,windows) # Disable cmd.exe window on open

	target_link_libraries(ui_win32 PUBLIC ${LIBS})
	set(LIBUI_LIB ui_win32)
elseif(APPLE)
	file(GLOB LIBUI_DARWIN ABSOLUTE ${PROJECT_SOURCE_DIR} "darwin/*.m")
	list(FILTER LIBUI_DARWIN EXCLUDE REGEX ".*OLD_table\\.m$")

	add_library(ui_darwin STATIC ${LIBUI_COMMON} ${LIBUI_DARWIN}
		${CMAKE_CURRENT_SOURCE_DIR}/extras/favicon/darwin.c
	)
	target_include_directories(ui_darwin PRIVATE ${PROJECT_SOURCE_DIR}/darwin ${PROJECT_SOURCE_DIR} PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_compile_options(ui_darwin
	    PRIVATE ${LIBUI_COMMON_FLAGS}
	)

	target_link_libraries(ui_darwin PUBLIC
	  "$<LINK_LIBRARY:FRAMEWORK,CoreFoundation,Foundation,AppKit,CoreText,CoreGraphics>"
	)
	set(LIBUI_LIB ui_darwin)
elseif(UNIX)
	file(GLOB LIBUI_GTK3 ABSOLUTE ${PROJECT_SOURCE_DIR} "unix/*.c")
	list(FILTER LIBUI_GTK3 EXCLUDE REGEX ".*OLD_table\\.c$")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 IMPORTED_TARGET GLOBAL gtk+-3.0)

	add_library(ui_gtk3 STATIC ${LIBUI_COMMON} ${LIBUI_GTK3}
		${CMAKE_CURRENT_SOURCE_DIR}/extras/favicon/linux.c
		${CMAKE_CURRENT_SOURCE_DIR}/extras/label.c
	)
	target_include_directories(ui_gtk3 PRIVATE ${PROJECT_SOURCE_DIR}/unix ${PROJECT_SOURCE_DIR} PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_link_libraries(ui_gtk3 PkgConfig::GTK3)
	target_compile_options(ui_gtk3
	    PRIVATE ${LIBUI_COMMON_FLAGS}
	)
	set(LIBUI_LIB ui_gtk3)
else()
	message(FATAL_ERROR "platform not supported")
endif()

add_library(LibUI::LibUI ALIAS ${LIBUI_LIB})

if(LIBUI_INCLUDE_INSTALL)
	configure_file(libui.pc.in ui.pc @ONLY)
	include(GNUInstallDirs)
	install(
		TARGETS ${LIBUI_LIB}
	)
	install(
		FILES
		${PROJECT_SOURCE_DIR}/include/ui.h
		${PROJECT_SOURCE_DIR}/include/ui_darwin.h
		${PROJECT_SOURCE_DIR}/include/ui_windows.h
		${PROJECT_SOURCE_DIR}/include/ui_unix.h
		${PROJECT_SOURCE_DIR}/include/ui_extras.h
		DESTINATION include/ui
	)
	install(
		FILES ${CMAKE_CURRENT_BINARY_DIR}/ui.pc
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
	)
endif()

if(LIBUI_COMPILE_EXAMPLES)
	add_executable(hello-world examples/hello-world/main.c)
	target_link_libraries(hello-world LibUI::LibUI)

	add_executable(controlgallery examples/controlgallery/main.c)
	target_link_libraries(controlgallery LibUI::LibUI)
endif()
