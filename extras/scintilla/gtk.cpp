// A simple demonstration application using Scintilla
#include <stdio.h>
#include <ui.h>
#include <gtk/gtk.h>
#include <Scintilla.h>
#include <ScintillaTypes.h>
#include <ScintillaStructures.h>
#include "../../include/ui_unix.h"

#include "ui_scintilla.h"
#include <ScintillaWidget.h>

#define uiScintilla(this) ((uiScintilla *)this)
#define uiScintillaSignature 0x1234

struct uiScintilla {
	struct uiUnixControl c;
	GtkWidget *widget;
	ScintillaObject *sci;
};

uiUnixControlAllDefaults(uiScintilla)

_UI_EXTERN uiScintilla *uiNewScintilla() {
	struct uiScintilla *s;

	uiUnixNewControl(uiScintilla, s);

	s->widget = scintilla_new();
	s->sci = SCINTILLA(s->widget);

	return s;
}

uintptr_t uiScintillaSendMessage(uiScintilla *s, uint32_t code, uintptr_t w, uintptr_t l) {
	return scintilla_object_send_message(s->sci, code, w, l);
}

#if 1
void uiScintillaGetRange(uiScintilla *s, int start, int end, char *text) {
	struct Scintilla::TextRange tr;
	tr.chrg.cpMin = start;
	tr.chrg.cpMax = end;
	tr.lpstrText = reinterpret_cast<char *>(text);
	scintilla_object_send_message(s->sci, SCI_GETTEXTRANGE, 0, reinterpret_cast<gintptr>(&tr));
}

void uiScintillaSetText(uiScintilla *s, const char *text) {
	scintilla_object_send_message(s->sci, SCI_SETTEXT, strlen(text), reinterpret_cast<gintptr>(text));
}

void uiScintillaAppend(uiScintilla *s, const char *text) {
	scintilla_object_send_message(s->sci, SCI_APPENDTEXT, strlen(text), reinterpret_cast<gintptr>(text));
}

int uiScintillaGetLength(uiScintilla *s) {
	return scintilla_object_send_message(s->sci, SCI_GETLENGTH, 0, 0);
}

char *uiScintillaText(uiScintilla *s) {
	int len = uiScintillaGetLength(s);
	char *text = (char *)malloc((size_t)len);
	uiScintillaGetRange(s, 0, len, text);
	return text;
}
#endif
