#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

GtkWidget *scintilla_new() {
	GtkWidget *view;
	PangoFontDescription *font_desc;

	view = gtk_source_view_new();
	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(view), TRUE);

	font_desc = pango_font_description_from_string("Monospace 12");
	gtk_widget_override_font(view, font_desc);

	pango_font_description_free(font_desc);

	return view;
}

int main(int argc, char **argv) {
	GtkWidget *window;
	GtkWidget *scintilla;

	GtkSettings *settings;

	gtk_init(&argc, &argv);

	settings = gtk_settings_get_default();
	g_object_set(settings, "gtk-application-prefer-dark-theme", TRUE, NULL);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

	scintilla = scintilla_new();
	gtk_container_add(GTK_CONTAINER(window), scintilla);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}

