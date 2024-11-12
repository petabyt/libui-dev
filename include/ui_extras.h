// Set an icon from an ICO icon buffer.
_UI_EXTERN void uiWindowSetIcon(uiWindow *w, const void *data, size_t length);

// WIP - not supported on all platforms yet
typedef struct uiScroll uiScroll;
_UI_EXTERN uiScroll *uiNewScroll();
_UI_EXTERN void uiScrollSetChild(uiScroll *scroll, uiControl *ctl);

// Experimental - use uiAttribute API on normal widgets
_UI_EXTERN void uiLabelSetAttribute(uiLabel *label, uiAttribute *attr);
