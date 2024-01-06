#include <stdint.h>
#include <ui.h>

#ifdef __unix__

#include <unix/uipriv_unix.h>
#include <unix/attrstr.h>
#include <gtk/gtk.h>

struct uiLabel {
	uiUnixControl c;
	GtkWidget *widget;
	GtkMisc *misc;
	GtkLabel *label;
};

PangoAttrList *uiprivAttributedStringToPangoAttrList(uiDrawTextLayoutParams *p);

struct attr {
	uiAttribute *val;
	size_t start;
	size_t end;
	struct attr *prev;
	struct attr *next;
};

struct uiprivAttrList {
	struct attr *first;
	struct attr *last;
};

struct uiAttributedString {
	char *s;
	size_t len;

	uiprivAttrList *attrs;

	// indiscriminately keep a UTF-16 copy of the string on all platforms so we can hand this off to the grapheme calculator
	// this ensures no one platform has a speed advantage (sorry GTK+)
	uint16_t *u16;
	size_t u16len;

	size_t *u8tou16;
	size_t *u16tou8;

	// this is lazily created to keep things from getting *too* slow
	uiprivGraphemes *graphemes;
};


_UI_EXTERN void uiLabelSetAttribute(uiLabel *label, uiAttribute *attr)
{
	struct uiDrawTextLayoutParams params;
	params.String = uiNewAttributedString("");

	uiAttributedStringSetAttribute(params.String, attr, 0, -1);

	PangoAttrList *pango_list = uiprivAttributedStringToPangoAttrList(&params);

	PangoAttrList *old_list = gtk_label_get_attributes(GTK_LABEL(label->widget));

	GSList *attr_list = pango_attr_list_get_attributes(old_list);

	for (; attr_list != NULL; attr_list = attr_list->next) {
		pango_attr_list_insert(pango_list, attr_list->data);
	}

	gtk_label_set_attributes(GTK_LABEL(label->widget), pango_list);
}

#endif
