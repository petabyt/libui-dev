// 8 september 2015

typedef struct uiScroll uiScroll;
#define uiScroll(this) ((uiScroll *) (this))

#define uiScrollSignature 0x083983

#include "../libui-ng/windows/uipriv_windows.hpp"

struct boxChild {
	uiControl *c;
	int stretchy;
	int width;
	int height;
};

struct uiScroll {
	uiWindowsControl c;
	HWND hwnd;
	std::vector<struct boxChild> *controls;
};

uiWindowsControlDefaultHandle(uiScroll)
uiWindowsControlDefaultParent(uiScroll)
uiWindowsControlDefaultSetParent(uiScroll)
uiWindowsControlDefaultToplevel(uiScroll)
uiWindowsControlDefaultVisible(uiScroll)
uiWindowsControlDefaultShow(uiScroll)
uiWindowsControlDefaultHide(uiScroll)
uiWindowsControlDefaultEnabled(uiScroll)
uiWindowsControlDefaultEnable(uiScroll)
uiWindowsControlDefaultDisable(uiScroll)
uiWindowsControlDefaultSetParentHWND(uiScroll)
uiWindowsControlDefaultLayoutRect(uiScroll)
uiWindowsControlDefaultAssignControlIDZOrder(uiScroll)

static void uiScrollMinimumSize(uiWindowsControl *c, int *width, int *height) {

	*width = 0;
	*height = 0;

	printf("min size\n");
	// ...
}

static void uiScrollSyncEnableState(uiWindowsControl *c, int enabled) {
	uiScroll *b = (uiScroll *)c;

	if (uiWindowsShouldStopSyncEnableState(uiWindowsControl(b), enabled))
		return;
	for (const struct boxChild &bc : *(b->controls))
		uiWindowsControlSyncEnableState(uiWindowsControl(bc.c), enabled);
}

static void uiScrollDestroy(uiControl *c) {
	// ...
}

static void uiScrollChildVisibilityChanged(uiWindowsControl *c) {
	// ...
}

static void uiScrollMinimumSizeChanged(uiWindowsControl *c) {
	// asd
}

static void onResize(uiWindowsControl *c)
{
	uiScroll *t = (uiScroll *)c;

	RECT r;
	LONG_PTR controlID;
	HWND insertAfter;

	// first move the tab control itself
	uiWindowsEnsureGetClientRect(t->hwnd, &r);
	uiWindowsEnsureMoveWindowDuringResize(t->hwnd, r.left, r.top, r.right - r.left, r.bottom - r.top);

	printf("%d %d %d %d\n", r.left, r.top, r.right, r.bottom);

	//controlID = 100;
	//insertAfter = NULL;
	//uiWindowsEnsureMoveWindowDuringResize(page->hwnd, r.left, r.top, r.right - r.left, r.bottom - r.top);
	//uiWindowsEnsureAssignControlIDZOrder(page->hwnd, &controlID, &insertAfter);

	printf("on resize\n");
}

extern "C" {
	_UI_EXTERN uiScroll *uiNewScroll();
	_UI_EXTERN void uiScrollSetChild(uiScroll *scroll, uiControl *ctl);
}

void uiScrollSetChild(uiScroll *scroll, uiControl *ctl) {
	struct boxChild bc;

	bc.c = ctl;

	uiControlSetParent(bc.c, uiControl(scroll));
	uiWindowsControlSetParentHWND(uiWindowsControl(bc.c), scroll->hwnd);

	scroll->controls->push_back(bc);
}

struct containerInit {
	uiWindowsControl *c;
	void (*onResize)(uiWindowsControl *);
};

uiScroll *uiNewScroll() {
	uiScroll *s;

	printf("new\n");

	uiWindowsNewControl(uiScroll, s);

	//s->hwnd = uiWindowsMakeContainer(uiWindowsControl(s), onResize);

	struct containerInit init;

	// TODO onResize cannot be NULL
	init.c = uiWindowsControl(s);
	init.onResize = onResize;

	uiWindowsEnsureCreateControlHWND(WS_EX_CONTROLPARENT,
		containerClass, L"",
		0,
		hInstance, &init,
		FALSE);

	s->controls = new std::vector<struct boxChild>;
	
	return s;
}
