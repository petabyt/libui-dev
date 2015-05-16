# 22 april 2015

osCFILES = \
	windows/alloc.c \
	windows/button.c \
	windows/checkbox.c \
	windows/container.c \
	windows/control.c \
	windows/debug.c \
	windows/entry.c \
	windows/group.c \
	windows/init.c \
	windows/label.c \
	windows/main.c \
	windows/menu.c \
	windows/parent.c \
	windows/resize.c \
	windows/tab.c \
	windows/text.c \
	windows/util.c \
	windows/utilwin.c \
	windows/window.c

osHFILES = \
	windows/uipriv_windows.h

# thanks ebassi in irc.gimp.net/#gtk+
osCFLAGS = \
	-D_UI_EXTERN='__declspec(dllexport) extern'

osLDFLAGS = \
	-static-libgcc \
	-luser32 -lkernel32 -lgdi32 -luxtheme -lmsimg32 -lcomdlg32 -lole32 -loleaut32 -loleacc -luuid

osLDWarnUndefinedFlags = -Wl,--no-undefined -Wl,--no-allow-shlib-undefined

osLIBSUFFIX = .dll
osEXESUFFIX = .exe

ifeq ($(ARCH),amd64)
	CC = x86_64-w64-mingw32-gcc
	RC = x86_64-w64-mingw32-windres
	archmflag = -m64
else
	CC = i686-w64-mingw32-gcc
	RC = i686-w64-mingw32-windres
	archmflag = -m32
endif

ifeq ($(PROFILE),1)
	osCFILES += windows/profiler.c
	osCFLAGS += -finstrument-functions
	osLDFLAGS += -finstrument-functions
endif