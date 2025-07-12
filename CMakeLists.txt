cmake_minimum_required(VERSION 3.15)
project(ui)

option(LIBUI_COMPILE_EXAMPLES "Compile example programs and tests" OFF)
option(LIBUI_INCLUDE_INSTALL "Include install() declaration" OFF)
option(LIBUI_COMPILE_GTK3 "Compile GTK3 on Linux" ON)
option(LIBUI_COMPILE_QT5 "Compile Qt5 on Linux" OFF)
option(LIBUI_COMPILE_GL "glArea support" ON)

file(GLOB LIBUI_COMMON ABSOLUTE ${PROJECT_SOURCE_DIR} "common/*.c")
list(FILTER LIBUI_COMMON EXCLUDE REGEX ".*OLD_table\\.c$")

set(LIBUI_COMMON_FLAGS -fvisibility=hidden -fdiagnostics-color=always -D_FILE_OFFSET_BITS=64 -Wall -Winvalid-pch -Wextra -Wpedantic -Wno-unused-parameter -Wno-switch -D_UI_STATIC -Dlibui_EXPORTS)

if(WIN32)
	file(GLOB LIBUI_WIN ABSOLUTE ${PROJECT_SOURCE_DIR} "windows/*.cpp")
	if(NOT LIBUI_COMPILE_GL)
		list(FILTER LIBUI_GTK3 EXCLUDE REGEX ".*openglarea\\.cpp$")
	endif()

	add_library(ui_win32 STATIC ${LIBUI_COMMON} ${LIBUI_WIN}
		${CMAKE_CURRENT_SOURCE_DIR}/extras/favicon/win.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/extras/scroll.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/extras/win_init.cpp
	)
	target_include_directories(ui_win32 PRIVATE ${PROJECT_SOURCE_DIR}/windows ${PROJECT_SOURCE_DIR} PUBLIC ${PROJECT_SOURCE_DIR}/include)
	target_compile_options(ui_win32
	    PRIVATE ${LIBUI_COMMON_FLAGS}
	)

	list(APPEND LIBS -luser32 -lkernel32 -lgdi32 -lcomctl32 -luxtheme -lmsimg32 -lcomdlg32 -ld2d1 -ldwrite -lole32 -loleaut32 -loleacc -lopengl32)
	list(APPEND LIBS -lstdc++ -lgcc -static -lpthread -lssp -lurlmon -luuid)
	#list(APPEND LIBS -Wl,-subsystem,windows) # Disable cmd.exe window on open

	target_link_libraries(ui_win32 PUBLIC ${LIBS})
	set(LIBUI_LIB ui_win32)
elseif(APPLE)
	file(GLOB LIBUI_DARWIN ABSOLUTE ${PROJECT_SOURCE_DIR} "darwin/*.m")
	list(FILTER LIBUI_DARWIN EXCLUDE REGEX ".*OLD_table\\.m$")
	if(NOT LIBUI_COMPILE_GL)
		list(FILTER LIBUI_GTK3 EXCLUDE REGEX ".*openglarea\\.m$")
	endif()

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
	if(LIBUI_COMPILE_GTK3)
		file(GLOB LIBUI_GTK3 ABSOLUTE ${PROJECT_SOURCE_DIR} "unix/*.c")
		list(FILTER LIBUI_GTK3 EXCLUDE REGEX ".*OLD_table\\.c$")
		if(NOT LIBUI_COMPILE_GL)
			list(FILTER LIBUI_GTK3 EXCLUDE REGEX ".*openglarea\\.c$")
		endif()

	    find_package(PkgConfig REQUIRED)
	    pkg_check_modules(GTK3 IMPORTED_TARGET GLOBAL gtk+-3.0)

		add_library(ui_gtk3 STATIC ${LIBUI_COMMON} ${LIBUI_GTK3}
			${CMAKE_CURRENT_SOURCE_DIR}/extras/favicon/linux.c
			${CMAKE_CURRENT_SOURCE_DIR}/extras/label.c
		)
		target_include_directories(ui_gtk3 PRIVATE ${PROJECT_SOURCE_DIR}/unix ${PROJECT_SOURCE_DIR} PUBLIC ${PROJECT_SOURCE_DIR}/include)
		target_link_libraries(ui_gtk3 PkgConfig::GTK3
			$<$<BOOL:${LIBUI_COMPILE_GL}>:-lwayland-client -lwayland-egl -lEGL>
		)
		target_compile_options(ui_gtk3
		    PRIVATE ${LIBUI_COMMON_FLAGS}
		)
		set(LIBUI_LIB ui_gtk3)
	elseif(LIBUI_COMPILE_QT5)
		find_package(PkgConfig REQUIRED)
		pkg_check_modules(QT IMPORTED_TARGET GLOBAL Qt5Widgets)
		file(GLOB LIBUI_QT5 ABSOLUTE ${PROJECT_SOURCE_DIR} "qt5/*.cpp")
		add_library(ui_qt5 SHARED ${LIBUI_COMMON} ${LIBUI_QT5})
		target_include_directories(ui_qt5 PRIVATE ${PROJECT_SOURCE_DIR}/qt5 ${PROJECT_SOURCE_DIR} PUBLIC ${PROJECT_SOURCE_DIR}/include)
		target_link_libraries(ui_qt5 PkgConfig::QT)
		target_compile_options(ui_qt5
		    PRIVATE ${LIBUI_COMMON_FLAGS}
		)
		set(LIBUI_LIB ui_qt5)
		#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_DEBUG")
		#else()
		#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_DEBUG")
		#endif()
	endif()
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
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/subprojects/cmocka-1.1.5 ${CMAKE_CURRENT_BINARY_DIR}/cmocka)
	
	function(add_ex name)
		if(WIN32)
			add_executable(${name} ${ARGN} examples/resources.rc)
		else()
			add_executable(${name} ${ARGN})
		endif()
		target_link_libraries(${name} LibUI::LibUI cmocka -lm)
	endfunction()

	add_ex(hello-world examples/hello-world/main.c)
	add_ex(controlgallery examples/controlgallery/main.c)
	add_ex(histogram examples/histogram/main.c)
	add_ex(drawtext examples/drawtext/main.c)
	add_ex(datetime examples/datetime/main.c)
	add_ex(window examples/window/main.c)
	add_ex(draw examples/draw/main.c)
	add_ex(test test/drawtests.c test/images.c test/main.c test/menus.c test/page1.c test/page2.c test/page3.c test/page4.c test/page5.c test/page6.c test/page7.c test/page7a.c test/page7b.c test/page7c.c test/page11.c test/page12.c test/page13.c test/page14.c test/page15.c test/page16.c test/page17.c test/spaced.c)
	add_ex(unit test/unit/main.c test/unit/init.c test/unit/slider.c test/unit/spinbox.c test/unit/label.c test/unit/button.c test/unit/combobox.c test/unit/checkbox.c test/unit/radiobuttons.c test/unit/entry.c test/unit/menu.c test/unit/progressbar.c test/unit/drawmatrix.c test/unit/window.c)

	if(LIBUI_COMPILE_GL)
		option(GLEW_X11 "X11 mode" ON)
		option(GLEW_EGL "EGL mode" ON)
		option(BUILD_SHARED_LIBS "install/link shared instead of static libs" OFF)
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../glew/build/cmake ${CMAKE_CURRENT_BINARY_DIR}/glew)

		add_executable(opengl examples/opengl/main.c)
		target_link_libraries(opengl LibUI::LibUI glew_s -lm)
	endif()
endif()
