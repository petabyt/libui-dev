#include <ui.h>

int onClosing(uiWindow *w, void *data)
{
	uiQuit();
	return 1;
}

void click(uiButton *btn, void *data) {
	uiScintilla *s = (uiScintilla *)data;
	printf("Helol %d\n", uiScintillaGetLength(s));
}

int main(void)
{
	AttachConsole(-1);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	Scintilla::Internal::RegisterClasses(hInstance);

	uiInitOptions o = {0};
	const char *err;
	uiWindow *w;
	uiLabel *l;

	err = uiInit(&o);
	if (err != NULL) {
		fprintf(stderr, "Error initializing libui-ng: %s\n", err);
		uiFreeInitError(err);
		return 1;
	}

	// Create a new window
	w = uiNewWindow("Hello World!", 1000, 1000, 0);
	uiWindowOnClosing(w, onClosing, NULL);

	uiButton *btn = uiNewButton("Hello");

	uiScintilla *s = uiNewScintilla();
	
	uiBox *v = uiNewVerticalBox();
	uiBoxAppend(v, uiControl(s), 1);
	uiBoxAppend(v, uiControl(btn), 0);

	uiButtonOnClicked(btn, click, (void *)s);

	uiWindowSetChild(w, uiControl(v));

	uiControlShow(uiControl(w));
	uiMain();
	uiUninit();
	return 0;
}

