// 18 april 2015
#include "uipriv_unix.h"

void uiSetAppMetadata(const char *name, const char *version, const char *package) {
	(void)version;
	GtkApplication *app = gtk_application_new(package, G_APPLICATION_FLAGS_NONE);
	(void)app; // leak
	g_set_application_name(name);
}

void uiprivSetMargined(GtkContainer *c, int margined)
{
	if (margined)
		gtk_container_set_border_width(c, uiprivGTKXMargin);
	else
		gtk_container_set_border_width(c, 0);
}
