// Set an icon from an ICO icon buffer.
_UI_EXTERN void uiWindowSetIcon(uiWindow *w, const void *data, size_t length);

// WIP - not supported on all platforms yet
typedef struct uiScroll uiScroll;
_UI_EXTERN uiScroll *uiNewScroll();
_UI_EXTERN void uiScrollSetChild(uiScroll *scroll, uiControl *ctl);

// Experimental - use uiAttribute API on normal widgets
_UI_EXTERN void uiLabelSetAttribute(uiLabel *label, uiAttribute *attr);

// ----------- uigl -----------------

typedef struct uiOpenGLArea uiOpenGLArea;
typedef struct uiOpenGLAreaHandler uiOpenGLAreaHandler;
typedef struct uiOpenGLAttributes uiOpenGLAttributes;
#define uiOpenGLArea(this) ((uiOpenGLArea *) (this))

struct uiOpenGLAreaHandler {
	void (*DrawGL)(uiOpenGLAreaHandler *, uiOpenGLArea *, double width, double height);
	void (*MouseEvent)(uiOpenGLAreaHandler *, uiOpenGLArea *, uiAreaMouseEvent *);
	void (*MouseCrossed)(uiOpenGLAreaHandler *, uiOpenGLArea *, int left);
	void (*DragBroken)(uiOpenGLAreaHandler *, uiOpenGLArea *);
	int (*KeyEvent)(uiOpenGLAreaHandler *, uiOpenGLArea *, uiAreaKeyEvent *);
	void (*InitGL)(uiOpenGLAreaHandler *, uiOpenGLArea *);
};

_UI_EXTERN void uiOpenGLAreaBeginUserWindowMove(uiOpenGLArea *a);
_UI_EXTERN void uiOpenGLAreaBeginUserWindowResize(uiOpenGLArea *a, uiWindowResizeEdge edge);
_UI_EXTERN void uiOpenGLAreaSetVSync(uiOpenGLArea *a, int v);
_UI_EXTERN void uiOpenGLAreaQueueRedrawAll(uiOpenGLArea *a);
_UI_EXTERN void uiOpenGLAreaMakeCurrent(uiOpenGLArea *a);
_UI_EXTERN void uiOpenGLAreaSwapBuffers(uiOpenGLArea *a);
_UI_EXTERN uiOpenGLArea *uiNewOpenGLArea(uiOpenGLAreaHandler *ah, uiOpenGLAttributes *attribs);

_UI_ENUM(uiOpenGLAttribute) {
	// The default value is either listed explicitly or the first of the given possible values

	// Desired bit depths of the default framebuffer (integer number of bits).
	uiOpenGLAttributeRedBits,     // = 8
	uiOpenGLAttributeGreenBits,   // = 8
	uiOpenGLAttributeBlueBits,    // = 8
	uiOpenGLAttributeAlphaBits,   // = 8
	uiOpenGLAttributeDepthBits,   // = 24
	uiOpenGLAttributeStencilBits, // = 8

	// Stereoscopic rendering (boolean, 0 or 1).
	uiOpenGLAttributeStereo,

	// Number of samples to use for multisampling (integer number of samples).
	uiOpenGLAttributeSamples,

	// Whether the framebuffer should be sRGB capable (boolean, 0 or 1) - ignored on macOS.
	uiOpenGLAttributeSRGBCapable,

	// Whether the framebuffer should be double-buffered (boolean, 1 or 0).
	uiOpenGLAttributeDoubleBuffer,

	// Whether to use OpenGL ES; if false, OpenGL is used (0 or 1) - ignored on macOS.
	uiOpenGLAttributeUseOpenGLES,

	// Major and minor versions of OpenGL/OpenGL ES (integer version number).
	uiOpenGLAttributeMajorVersion, // = 1
	uiOpenGLAttributeMinorVersion, // = 0

	// Whether the OpenGL context should be forward-compatible, with no deprecated functionality;
	// OpenGL 3.0+ only (boolean, 0 or 1) - ignored on macOS and generally dicouraged.
	uiOpenGLAttributeForwardCompat,

	// Whether to create a debug context (boolean, 0 or 1) - ignored on macOS.
	uiOpenGLAttributeDebugContext,

	// Whether to use the Compatibility Profile; if false, the Core Profile is used (boolean, 0 or
	// 1) - on macOS the Core Profile is always used
	uiOpenGLAttributeCompatProfile,

	// Whether to use Robustness (boolean, 0 or 1) - ignored on macOS.
	uiOpenGLAttributeRobustness,
};

_UI_EXTERN uiOpenGLAttributes *uiNewOpenGLAttributes();
_UI_EXTERN void uiFreeOpenGLAttributes(uiOpenGLAttributes *attribs);
_UI_EXTERN void uiOpenGLAttributesSetAttribute(uiOpenGLAttributes *attribs, uiOpenGLAttribute attribute, int value);
