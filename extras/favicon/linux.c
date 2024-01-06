#include <stdint.h>
#include <ui.h>

#ifdef __unix__

#include <unix/uipriv_unix.h>
#include <gtk/gtk.h>

struct uiWindow {
	uiUnixControl c;

	GtkWidget *widget;
	GtkContainer *container;
	GtkWindow *window;
};

_UI_EXTERN void uiWindowSetIcon(uiWindow *w, const void *data, size_t length)
{
	GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
	gdk_pixbuf_loader_write(loader, data, length, NULL);
	gdk_pixbuf_loader_close(loader, NULL);
	GdkPixbuf *pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
	gtk_window_set_icon(GTK_WINDOW(w->window), pixbuf);
}

#endif
