#include <stdio.h>
#include <ui.h>

int onClosing(uiWindow *w, void *data)
{
	uiQuit();
	return 1;
}

static void clickevent(uiButton *button, void *arg) {
	puts("Hello");
}

int main(void)
{
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
	w = uiNewWindow("Hello World!", 300, 300, 0);
	uiWindowOnClosing(w, onClosing, NULL);

	uiBox *frame = uiNewVerticalBox();
	uiBoxSetPadded(frame, 1);

	// --------------------------

	uiTab *tab;
	uiButton *btn;

	uiBox *box1 = uiNewVerticalBox();
	uiBoxSetPadded(box1, 1);

	btn = uiNewButton("Hello page 1");

	uiBoxAppend(box1, uiControl(btn), 0);
	uiButtonOnClicked(btn, clickevent, 0);

	btn = uiNewButton("Another button page 1");
	uiBoxAppend(box1, uiControl(btn), 0);

	uiBox *box2 = uiNewVerticalBox();
	uiBoxSetPadded(box2, 1);

	btn = uiNewButton("Hello");

	uiBoxAppend(box2, uiControl(btn), 0);
	uiButtonOnClicked(btn, clickevent, 0);

	btn = uiNewButton("asd");
	uiBoxAppend(box2, uiControl(btn), 0);

	tab = uiNewTab();
	uiTabAppend(tab, "tab1", uiControl(box1));
	uiTabAppend(tab, "tab2", uiControl(box2));

	uiBoxAppend(frame, uiControl(tab), 0);

	// --------------------------

	uiWindowSetChild(w, uiControl(frame));
	uiControlShow(uiControl(w));
	uiMain();
	uiUninit();
	return 0;
}
