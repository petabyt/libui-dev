#include <windows/uipriv_windows.hpp>

struct uiWindow {
	uiWindowsControl c;
	HWND hwnd;
	HMENU menubar;
};

extern "C"
void uiWindowSetIcon(uiWindow *w, const void *data, size_t length)
{
	// https://en.wikipedia.org/wiki/ICO_(file_format)#Header
	uint16_t *header = (uint16_t *)data;
	if (header[1] != 0x0 && header[1] != 0x1) {
		return;
	}

	int offset = 3 * sizeof(uint16_t) + 16 * header[2];

	HICON icon = CreateIconFromResource(((PBYTE)data) + offset, length, TRUE, 0x30000);

	HRESULT rc = SendMessage(w->hwnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)icon);
}
