#include "uipriv_unix.h"
#include "areacommon.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <gdk/gdkx.h>
#include <gdk/gdkwayland.h>
#include <wayland-egl.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define openGLAreaWidgetType (openGLAreaWidget_get_type())
#define openGLAreaWidget(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), openGLAreaWidgetType, openGLAreaWidget))
#define isOpenGLAreaWidget(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), openGLAreaWidgetType, openGLAreaWidgetClass))
#define openGLAreaWidgetClass(class) (G_TYPE_CHECK_CLASS_CAST((class), openGLAreaWidgetType, openGLAreaWidgetClass))
#define isOpenGLAreaWidgetClass(class) (G_TYPE_CHECK_CLASS_TYPE((class), openGLAreaWidget))
#define getOpenGLAreaWidgetClass(class) (G_TYPE_INSTANCE_GET_CLASS((class), openGLAreaWidgetType, openGLAreaWidgetClass))

#define EGL_ATTRIBUTE_LIST_SIZE 256

static uiOpenGLArea *globalOpenGLArea = NULL;

// Global Wayland subcompositor (shared across instances)
static struct wl_subcompositor *global_subcompositor = NULL;

typedef void (*eglSwapIntervalEXTFn)(EGLDisplay, EGLint);
static eglSwapIntervalEXTFn uiEGLSwapIntervalEXT = NULL;
typedef EGLDisplay (*PFNEGLGETPLATFORMDISPLAYEXTPROC)(EGLenum platform, void *native_display, const EGLint *attrib_list);
typedef EGLSurface (*PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC)(EGLDisplay dpy, EGLConfig config, void *native_window, const EGLint *attrib_list);
static PFNEGLGETPLATFORMDISPLAYEXTPROC s_eglGetPlatformDisplay = NULL;
static PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC s_eglCreatePlatformWindowSurface = NULL;

void load_extensions(void) {
    uiEGLSwapIntervalEXT = (eglSwapIntervalEXTFn)eglGetProcAddress("eglSwapIntervalEXT");
    s_eglGetPlatformDisplay = (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    s_eglCreatePlatformWindowSurface = (PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC)eglGetProcAddress("eglCreatePlatformWindowSurfaceEXT");
}

static int check_egl_extension(const char *extensions, const char *extName) {
    if (!extensions || !extName) return 0;
    const char *start = extensions;
    size_t len = strlen(extName);
    while (*start) {
        if (strncmp(start, extName, len) == 0 && (start[len] == ' ' || start[len] == '\0'))
            return 1;
        start += strcspn(start, " ") + 1;
    }
    return 0;
}

typedef struct openGLAreaWidget openGLAreaWidget;
typedef struct openGLAreaWidgetClass openGLAreaWidgetClass;

struct openGLAreaWidget {
    GtkDrawingArea parent_instance;
    uiOpenGLArea *a;
    uiprivClickCounter cc;
};

struct openGLAreaWidgetClass {
    GtkDrawingAreaClass parent_class;
};

struct uiOpenGLArea {
    uiUnixControl c;
    GtkWidget *widget;
    GtkWidget *swidget;
    GtkContainer *scontainer;
    GtkScrolledWindow *sw;
    GtkWidget *areaWidget;
    GtkDrawingArea *drawingArea;
    openGLAreaWidget *area;
    uiOpenGLAreaHandler *ah;
    gboolean scrolling;
    int scrollWidth;
    int scrollHeight;
    uiprivClickCounter *cc;
    GdkEventButton *dragevent;
    uiOpenGLAttributes *attribs;
    GdkDisplay *gdkDisplay;
    Display *display;
    XVisualInfo *visual;
    GLXContext ctx;
    int initialized;
    int supportsSwapInterval;
    
    EGLDisplay eglDisplay;
    EGLContext eglContext;
    EGLSurface eglSurface;
    EGLConfig eglConfig;
    gboolean isWayland;
    struct wl_egl_window *wl_egl_window;
    gboolean contextInitialized;
    struct wl_surface *wl_surface;      // Added for Wayland subsurface
    struct wl_subsurface *wl_subsurface; // Added for Wayland subsurface
};

G_DEFINE_TYPE(openGLAreaWidget, openGLAreaWidget, GTK_TYPE_DRAWING_AREA)

// Wayland registry listener to bind to wl_subcompositor
static void registry_handle_global(void *data, struct wl_registry *registry,
                                   uint32_t name, const char *interface, uint32_t version)
{
    if (strcmp(interface, wl_subcompositor_interface.name) == 0) {
        global_subcompositor = wl_registry_bind(registry, name, &wl_subcompositor_interface, 1);
    }
}

static void registry_handle_global_remove(void *data, struct wl_registry *registry, uint32_t name)
{
    // No-op
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_handle_global,
    .global_remove = registry_handle_global_remove,
};

static void init_wayland_subcompositor(uiOpenGLArea *a)
{
    if (global_subcompositor) return; // Already initialized

    struct wl_display *display = gdk_wayland_display_get_wl_display(a->gdkDisplay);
    if (!display) {
        uiprivUserBug("Failed to get Wayland display for subcompositor initialization!");
        return;
    }

    struct wl_registry *registry = wl_display_get_registry(display);
    if (!registry) {
        uiprivUserBug("Failed to get Wayland registry for subcompositor initialization!");
        return;
    }

    wl_registry_add_listener(registry, &registry_listener, NULL);
    wl_display_roundtrip(display); // Ensure the listener is called

    if (!global_subcompositor) {
        uiprivUserBug("Failed to bind to wl_subcompositor!");
    }
}

static void initContext(uiOpenGLArea *a)
{
    uiOpenGLAttributes *attribs = a->attribs;
    a->gdkDisplay = gtk_widget_get_display(a->widget);

    if (!gtk_widget_get_realized(a->widget)) {
        uiprivUserBug("Widget is not realized in initContext!");
        return;
    }

    load_extensions();

    // Check platform extensions
    const char *eglExtensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    int has_platform_base = eglExtensions && check_egl_extension(eglExtensions, "EGL_EXT_platform_base");
    fprintf(stderr, "[libui] initContext: EGL_EXT_platform_base=%d\n", has_platform_base);

    // Initialize Wayland subcompositor if needed
    if (GDK_IS_WAYLAND_DISPLAY(a->gdkDisplay)) {
        init_wayland_subcompositor(a);
    }

    // Initialize EGL display
    a->isWayland = GDK_IS_WAYLAND_DISPLAY(a->gdkDisplay);
    if (a->isWayland) {
        void *native_display = gdk_wayland_display_get_wl_display(a->gdkDisplay);
        if (has_platform_base && s_eglGetPlatformDisplay &&
            check_egl_extension(eglExtensions, "EGL_KHR_platform_wayland")) {
            a->eglDisplay = s_eglGetPlatformDisplay(0x31D8, native_display, NULL);
        } else {
            a->eglDisplay = eglGetDisplay((EGLNativeDisplayType)native_display);
        }
    } else {
        void *native_display = gdk_x11_display_get_xdisplay(a->gdkDisplay);
        if (has_platform_base && s_eglGetPlatformDisplay &&
            check_egl_extension(eglExtensions, "EGL_KHR_platform_x11")) {
            a->eglDisplay = s_eglGetPlatformDisplay(0x31D5, native_display, NULL);
        } else {
            a->eglDisplay = eglGetDisplay((EGLNativeDisplayType)native_display);
        }
    }

    if (a->eglDisplay == EGL_NO_DISPLAY) {
        uiprivUserBug("Failed to get EGL display! Error: 0x%x", eglGetError());
        return;
    }

    EGLint major, minor;
    if (!eglInitialize(a->eglDisplay, &major, &minor)) {
        uiprivUserBug("Failed to initialize EGL! Error: 0x%x", eglGetError());
        return;
    }
    fprintf(stderr, "[libui] initContext: EGL initialized, version %d.%d\n", major, minor);

    if (!eglBindAPI(EGL_OPENGL_API)) {
        uiprivUserBug("Failed to bind EGL_OPENGL_API! Error: 0x%x", eglGetError());
        eglTerminate(a->eglDisplay);
        a->eglDisplay = EGL_NO_DISPLAY;
        return;
    }

    EGLint framebuffer_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 0,
        EGL_DEPTH_SIZE, 16,
        EGL_STENCIL_SIZE, 0,
        EGL_NONE
    };

    EGLint num_configs = 0;
    if (!eglChooseConfig(a->eglDisplay, framebuffer_attribs, &a->eglConfig, 1, &num_configs)) {
        uiprivUserBug("eglChooseConfig: 0x%x", eglGetError());
        eglTerminate(a->eglDisplay);
        a->eglDisplay = EGL_NO_DISPLAY;
    }

    // Create EGL context with fallback versions
    EGLint context_attribs[][16] = {
        { // Requested version
            EGL_CONTEXT_MAJOR_VERSION, attribs->MajorVersion,
            EGL_CONTEXT_MINOR_VERSION, attribs->MinorVersion,
            EGL_CONTEXT_OPENGL_PROFILE_MASK, attribs->CompatProfile ? EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT : EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
            attribs->DebugContext ? EGL_CONTEXT_OPENGL_DEBUG : EGL_NONE, attribs->DebugContext ? EGL_TRUE : 0,
            attribs->ForwardCompat ? EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE : EGL_NONE, attribs->ForwardCompat ? EGL_TRUE : 0,
            attribs->Robustness ? EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY : EGL_NONE, attribs->Robustness ? EGL_LOSE_CONTEXT_ON_RESET : 0,
            EGL_NONE
        },
        { // Fallback to 3.0
            EGL_CONTEXT_MAJOR_VERSION, 3,
            EGL_CONTEXT_MINOR_VERSION, 0,
            EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
            EGL_NONE
        },
        { // Fallback to 2.1
            EGL_CONTEXT_MAJOR_VERSION, 2,
            EGL_CONTEXT_MINOR_VERSION, 1,
            EGL_NONE
        }
    };

    int version_index;
    for (version_index = 0; version_index < 3; version_index++) {
        a->eglContext = eglCreateContext(a->eglDisplay, a->eglConfig, EGL_NO_CONTEXT, context_attribs[version_index]);
        if (a->eglContext != EGL_NO_CONTEXT) {
            break;
        }
        fprintf(stderr, "[libui] initContext: Failed to create OpenGL %d.%d context, error: 0x%x\n",
                context_attribs[version_index][1], context_attribs[version_index][3], eglGetError());
    }

    if (a->eglContext == EGL_NO_CONTEXT) {
        uiprivUserBug("Failed to create EGL context after all attempts! Error: 0x%x", eglGetError());
        eglTerminate(a->eglDisplay);
        a->eglDisplay = EGL_NO_DISPLAY;
        return;
    }

    a->supportsSwapInterval = eglGetProcAddress("eglSwapIntervalEXT") != NULL;
}

static void update_subsurface_position(uiOpenGLArea *a)
{
    if (!a->isWayland || !a->wl_subsurface) return;

    GtkWidget *toplevel = gtk_widget_get_toplevel(a->widget);
    if (!toplevel || !GTK_IS_WINDOW(toplevel)) return;

    GtkAllocation allocation;
    gtk_widget_get_allocation(a->widget, &allocation);

    gint x = 0, y = 0;
	gdk_window_get_origin (gtk_widget_get_window (a->widget), &x, &y);
    wl_subsurface_set_position(a->wl_subsurface, x, y);

    struct wl_surface *parent_surface = gdk_wayland_window_get_wl_surface(gtk_widget_get_window(toplevel));
    if (parent_surface) {
        wl_surface_commit(parent_surface);
    }
}

static void initSurface(uiOpenGLArea *a)
{
    GdkWindow *window = gtk_widget_get_window(a->widget);
    if (!window) {
        uiprivUserBug("No GdkWindow available for EGL surface creation!");
        return;
    }

    if (a->isWayland) {
        if (!GDK_IS_WAYLAND_WINDOW(window)) {
            uiprivUserBug("GdkWindow is not a Wayland window!");
            return;
        }

        GtkWidget *toplevel = gtk_widget_get_toplevel(a->widget);
        if (!toplevel || !GTK_IS_WINDOW(toplevel)) {
            uiprivUserBug("Failed to find toplevel window for Wayland subsurface!");
            return;
        }
        struct wl_surface *parent_surface = gdk_wayland_window_get_wl_surface(gtk_widget_get_window(toplevel));
        if (!parent_surface) {
            uiprivUserBug("Failed to get parent Wayland surface!");
            return;
        }

        // Create a new wl_surface for this widget
        a->wl_surface = wl_compositor_create_surface(gdk_wayland_display_get_wl_compositor(a->gdkDisplay));
        if (!a->wl_surface) {
            uiprivUserBug("Failed to create Wayland surface for OpenGL area!");
            return;
        }

		// Set the area this region can get events
        struct wl_region *region = wl_compositor_create_region(gdk_wayland_display_get_wl_compositor(a->gdkDisplay));
		wl_surface_set_input_region(a->wl_surface, region);

        // Create a subsurface for the OpenGL area
        if (!global_subcompositor) {
            uiprivUserBug("wl_subcompositor not available for creating subsurface!");
            wl_surface_destroy(a->wl_surface);
            a->wl_surface = NULL;
            return;
        }
        a->wl_subsurface = wl_subcompositor_get_subsurface(global_subcompositor, a->wl_surface, parent_surface);
        if (!a->wl_subsurface) {
            wl_surface_destroy(a->wl_surface);
            a->wl_surface = NULL;
            uiprivUserBug("Failed to create Wayland subsurface for OpenGL area!");
            return;
        }

        // Set the subsurface to desynchronized mode for better performance
        wl_subsurface_set_desync(a->wl_subsurface);

        // Get the widget's allocation
        GtkAllocation allocation;
        gtk_widget_get_allocation(a->widget, &allocation);
        gint scale = gdk_window_get_scale_factor(window);

        // Position the subsurface
        update_subsurface_position(a);

        // Create or resize Wayland EGL window
        if (a->wl_egl_window) {
            wl_egl_window_resize(a->wl_egl_window, allocation.width * scale, allocation.height * scale, 0, 0);
        } else {
            a->wl_egl_window = wl_egl_window_create(a->wl_surface, allocation.width * scale, allocation.height * scale);
        }
        if (!a->wl_egl_window) {
            wl_subsurface_destroy(a->wl_subsurface);
            a->wl_subsurface = NULL;
            wl_surface_destroy(a->wl_surface);
            a->wl_surface = NULL;
            uiprivUserBug("Failed to create Wayland EGL window!");
            return;
        }
        wl_surface_set_buffer_scale(a->wl_surface, scale);
        fprintf(stderr, "[libui] initSurface: Wayland EGL window created, surface=%p\n", (void *)a->wl_egl_window);

        if (s_eglCreatePlatformWindowSurface) {
            a->eglSurface = s_eglCreatePlatformWindowSurface(a->eglDisplay, a->eglConfig, a->wl_egl_window, NULL);
        } else {
            a->eglSurface = eglCreateWindowSurface(a->eglDisplay, a->eglConfig, (EGLNativeWindowType)a->wl_egl_window, NULL);
        }
    } else {
        if (!GDK_IS_X11_WINDOW(window)) {
            uiprivUserBug("GdkWindow is not an X11 window!");
            return;
        }
        Window xwin = gdk_x11_window_get_xid(window);
        if (s_eglCreatePlatformWindowSurface) {
            a->eglSurface = s_eglCreatePlatformWindowSurface(a->eglDisplay, a->eglConfig, &xwin, NULL);
        } else {
            a->eglSurface = eglCreateWindowSurface(a->eglDisplay, a->eglConfig, (EGLNativeWindowType)xwin, NULL);
        }
    }

    if (a->eglSurface == EGL_NO_SURFACE) {
        uiprivUserBug("Failed to create EGL surface! Error: 0x%x", eglGetError());
        if (a->wl_egl_window) {
            wl_egl_window_destroy(a->wl_egl_window);
            a->wl_egl_window = NULL;
        }
        if (a->wl_subsurface) {
            wl_subsurface_destroy(a->wl_subsurface);
            a->wl_subsurface = NULL;
        }
        if (a->wl_surface) {
            wl_surface_destroy(a->wl_surface);
            a->wl_surface = NULL;
        }
        return;
    }

    if (!eglMakeCurrent(a->eglDisplay, a->eglSurface, a->eglSurface, a->eglContext)) {
        uiprivUserBug("Failed to make EGL context current! Error: 0x%x", eglGetError());
        eglDestroySurface(a->eglDisplay, a->eglSurface);
        a->eglSurface = EGL_NO_SURFACE;
        if (a->wl_egl_window) {
            wl_egl_window_destroy(a->wl_egl_window);
            a->wl_egl_window = NULL;
        }
        if (a->wl_subsurface) {
            wl_subsurface_destroy(a->wl_subsurface);
            a->wl_subsurface = NULL;
        }
        if (a->wl_surface) {
            wl_surface_destroy(a->wl_surface);
            a->wl_surface = NULL;
        }
        return;
    }
    if (a->isWayland) {
        eglSwapInterval(a->eglDisplay, 0); // Prevent deadlocks on Wayland
    }
    a->contextInitialized = TRUE;
}

static void on_map(GtkWidget *widget, gpointer data)
{
    uiOpenGLArea *a = (uiOpenGLArea *)data;
    if (!a->contextInitialized) {
        initContext(a);
        if (a->eglContext != EGL_NO_CONTEXT && a->eglDisplay != EGL_NO_DISPLAY) {
            initSurface(a);
        }
    }

    if (!a->isWayland || !a->wl_surface) {
        return;
    }
}

static void on_realize(GtkWidget *widget, gpointer data)
{
    uiOpenGLArea *a = (uiOpenGLArea *)data;

    // Ensure the subsurface is positioned correctly after the widget is fully realized
    if (a->isWayland && a->wl_subsurface) {
        update_subsurface_position(a);
    }
}

static void on_size_allocate(GtkWidget *widget, GtkAllocation *allocation, gpointer data)
{
    uiOpenGLArea *a = (uiOpenGLArea *)data;
    if (a->contextInitialized && a->isWayland && a->wl_egl_window) {
        gint scale = gdk_window_get_scale_factor(gtk_widget_get_window(widget));
        wl_egl_window_resize(a->wl_egl_window, allocation->width * scale, allocation->height * scale, 0, 0);
        update_subsurface_position(a);
    }
}

static gboolean openGLAreaWidget_draw(GtkWidget *w, cairo_t *cr)
{
    uiOpenGLArea *a = openGLAreaWidget(w)->a;
    double width, height;

    if (!a->contextInitialized) {
        return FALSE;
    }

    uiprivLoadAreaSize((uiArea *)a, &width, &height);
    uiOpenGLAreaMakeCurrent(a);

    // Set viewport to match widget size
    glViewport(0, 0, (GLint)width, (GLint)height);

    if (!a->initialized) {
        (*(a->ah->InitGL))(a->ah, a);
        a->initialized = 1;
    }

    (*(a->ah->DrawGL))(a->ah, a, width, height);
    return FALSE;
}

uiprivUnixAreaAllDefaultsExceptDraw(openGLAreaWidget)

static void openGLAreaWidget_class_init(openGLAreaWidgetClass *class)
{
    G_OBJECT_CLASS(class)->dispose = openGLAreaWidget_dispose;
    G_OBJECT_CLASS(class)->finalize = openGLAreaWidget_finalize;
    G_OBJECT_CLASS(class)->set_property = openGLAreaWidget_set_property;
    G_OBJECT_CLASS(class)->get_property = openGLAreaWidget_get_property;

    GTK_WIDGET_CLASS(class)->size_allocate = openGLAreaWidget_size_allocate;
    GTK_WIDGET_CLASS(class)->draw = openGLAreaWidget_draw;
    GTK_WIDGET_CLASS(class)->button_press_event = openGLAreaWidget_button_press_event;
    GTK_WIDGET_CLASS(class)->button_release_event = openGLAreaWidget_button_release_event;
    GTK_WIDGET_CLASS(class)->motion_notify_event = openGLAreaWidget_motion_notify_event;
    GTK_WIDGET_CLASS(class)->enter_notify_event = openGLAreaWidget_enter_notify_event;
    GTK_WIDGET_CLASS(class)->leave_notify_event = openGLAreaWidget_leave_notify_event;
    GTK_WIDGET_CLASS(class)->key_press_event = openGLAreaWidget_key_press_event;
    GTK_WIDGET_CLASS(class)->key_release_event = openGLAreaWidget_key_release_event;
}

uiUnixControlAllDefaultsExceptDestroy(uiOpenGLArea)

static void uiOpenGLAreaDestroy(uiControl *c)
{
    uiOpenGLArea *a = uiOpenGLArea(c);
    eglMakeCurrent(a->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (a->eglSurface != EGL_NO_SURFACE) {
        eglDestroySurface(a->eglDisplay, a->eglSurface);
        a->eglSurface = EGL_NO_SURFACE;
    }
    if (a->wl_egl_window) {
        wl_egl_window_destroy(a->wl_egl_window);
        a->wl_egl_window = NULL;
    }
    if (a->wl_subsurface) {
        wl_subsurface_destroy(a->wl_subsurface);
        a->wl_subsurface = NULL;
    }
    if (a->wl_surface) {
        wl_surface_destroy(a->wl_surface);
        a->wl_surface = NULL;
    }
    if (a->eglContext != EGL_NO_CONTEXT) {
        eglDestroyContext(a->eglDisplay, a->eglContext);
        a->eglContext = EGL_NO_CONTEXT;
    }
    if (a->eglDisplay != EGL_NO_DISPLAY) {
        eglTerminate(a->eglDisplay);
        a->eglDisplay = EGL_NO_DISPLAY;
    }
    uiprivFree(a->attribs);
    g_object_unref(a->widget);
    globalOpenGLArea = NULL;
    uiFreeControl(uiControl(a));
}

void uiOpenGLAreaQueueRedrawAll(uiOpenGLArea *a)
{
    gtk_widget_queue_draw(a->widget);
}

void uiOpenGLAreaSetVSync(uiOpenGLArea *a, int v)
{
    if (!a->contextInitialized) {
        return;
    }
    uiOpenGLAreaMakeCurrent(a);
    if (uiEGLSwapIntervalEXT != NULL && a->supportsSwapInterval) {
        uiEGLSwapIntervalEXT(a->eglDisplay, v);
    }
}

void uiOpenGLAreaMakeCurrent(uiOpenGLArea *a)
{
    if (!a->contextInitialized || a->eglContext == EGL_NO_CONTEXT || a->eglSurface == EGL_NO_SURFACE) {
        return;
    }
    if (!eglMakeCurrent(a->eglDisplay, a->eglSurface, a->eglSurface, a->eglContext)) {
        uiprivUserBug("Failed to make EGL context current! Error: 0x%x", eglGetError());
    }
}

void uiOpenGLAreaSwapBuffers(uiOpenGLArea *a)
{
    if (!a->contextInitialized || a->eglSurface == EGL_NO_SURFACE) {
        return;
    }
    if (!eglSwapBuffers(a->eglDisplay, a->eglSurface)) {
        fprintf(stderr, "[libui] uiOpenGLAreaSwapBuffers: Failed to swap buffers! Error: 0x%x\n", eglGetError());
    }
}

void uiOpenGLAreaBeginUserWindowMove(uiOpenGLArea *a)
{
    uiAreaBeginUserWindowMove((uiArea *)a);
}

void uiOpenGLAreaBeginUserWindowResize(uiOpenGLArea *a, uiWindowResizeEdge edge)
{
    uiAreaBeginUserWindowResize((uiArea *)a, edge);
}

static void on_hide(GtkWidget *widget, gpointer data)
{
    uiOpenGLArea *a = (uiOpenGLArea *)data;
    if (!a->isWayland || !a->wl_surface) {
        return;
    }

	wl_surface_attach(a->wl_surface, NULL, 0, 0); // Detach buffer to stop rendering
	wl_surface_set_opaque_region(a->wl_surface, NULL); // Remove opaque region hint
	wl_surface_commit(a->wl_surface); // Apply changes
}

static void on_show(GtkWidget *widget, gpointer data) {
    // ...
}

uiOpenGLArea *uiNewOpenGLArea(uiOpenGLAreaHandler *ah, uiOpenGLAttributes *attribs)
{
    uiOpenGLArea *a;
    uiUnixNewControl(uiOpenGLArea, a);

    a->ah = ah;
    a->initialized = 0;
    a->contextInitialized = FALSE;
    a->scrolling = FALSE;
    a->attribs = uiprivAlloc(sizeof(*a->attribs), "uiOpenGLAttributes[]");
    memcpy(a->attribs, attribs, sizeof(*a->attribs));

    a->areaWidget = GTK_WIDGET(g_object_new(openGLAreaWidgetType, NULL));
    a->drawingArea = GTK_DRAWING_AREA(a->areaWidget);
    a->area = openGLAreaWidget(a->areaWidget);
    a->area->a = a;
    a->widget = a->areaWidget;

    gtk_widget_set_double_buffered(a->areaWidget, FALSE);
    gtk_widget_set_has_window(a->areaWidget, TRUE);
    gtk_widget_set_visible(a->areaWidget, TRUE);

    globalOpenGLArea = a;

    g_signal_connect(a->areaWidget, "map", G_CALLBACK(on_map), a);
    g_signal_connect(a->areaWidget, "realize", G_CALLBACK(on_realize), a);
    g_signal_connect(a->areaWidget, "size-allocate", G_CALLBACK(on_size_allocate), a);
    g_signal_connect(a->areaWidget, "unmap", G_CALLBACK(on_hide), a);

    return a;
}

