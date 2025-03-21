#include <windows/uipriv_windows.hpp>

/*
int extern_win_init(HICON hDefaultIcon, HCURSOR hDefaultCursor);
if (extern_win_init(hDefaultIcon, hDefaultCursor)) {
	ieLastErr("Error");
}
*/

int extern_win_init(HICON hDefaultIcon, HCURSOR hDefaultCursor) {
	// extern ATOM registerScrollClass(HICON hDefaultIcon, HCURSOR hDefaultCursor);
	// if (registerScrollClass(hDefaultIcon, hDefaultCursor) == 0) {
	// 	puts("registering uiScroll window class");
	// 	return 1;
	// }
#if 1
	// Set DPI awareness
	typedef void* fl_DPI_AWARENESS_CONTEXT;
	typedef BOOL(WINAPI* SetProcessDpiAwarenessContext_type)(fl_DPI_AWARENESS_CONTEXT);
	SetProcessDpiAwarenessContext_type fl_SetProcessDpiAwarenessContext =
		(SetProcessDpiAwarenessContext_type)GetProcAddress(LoadLibrary(L"User32.DLL"), "SetProcessDpiAwarenessContext");
	if (fl_SetProcessDpiAwarenessContext) {
		const fl_DPI_AWARENESS_CONTEXT fl_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 = (fl_DPI_AWARENESS_CONTEXT)(-4);
		BOOL is_dpi_aware = fl_SetProcessDpiAwarenessContext(fl_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

		typedef HRESULT(WINAPI* SetProcessDpiAwareness_type)(int);
		SetProcessDpiAwareness_type fl_SetProcessDpiAwareness =
			(SetProcessDpiAwareness_type)GetProcAddress(LoadLibrary(L"Shcore.DLL"), "SetProcessDpiAwareness");
		if (fl_SetProcessDpiAwareness) {
			const int fl_PROCESS_PER_MONITOR_DPI_AWARE = 2;
			if (fl_SetProcessDpiAwareness(fl_PROCESS_PER_MONITOR_DPI_AWARE) == S_OK) {
				is_dpi_aware = true;
			}
		} else {
			abort();
		}
	}

	HDC hdc = GetDC(NULL);
	if (hdc) {
		int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
		int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
		hMessageFont = CreateFont(
	        -(dpiY / 8),           // Height (negative to use character height)
	        0,                     // Width
	        0,                     // Escapement
	        0,                     // Orientation
	        FW_NORMAL,             // Weight
	        FALSE,                 // Italic
	        FALSE,                 // Underline
	        FALSE,                 // Strikeout
	        ANSI_CHARSET,          // Charset
	        OUT_DEFAULT_PRECIS,    // Output precision
	        CLIP_DEFAULT_PRECIS,   // Clipping precision
	        CLEARTYPE_QUALITY,     // Quality
	        DEFAULT_PITCH | FF_DONTCARE, // Pitch and Family
	        L"Segoe UI"            // Font name
	    );

	}
#endif

	//SetProcessDPIAware();

	return 0;
}
