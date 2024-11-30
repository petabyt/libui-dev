// A simple demonstration application using Scintilla
#include <stdio.h>
#include <ui.h>
#include <gtk/gtk.h>
#define GTK
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

	// scintilla_set_id(sci, 0);
	//   gtk_widget_set_usize(editor, 500, 300);

	//#define SSM(m, w, l) scintilla_send_message(sci, m, w, l)
	//
	//   SSM(SCI_STYLECLEARALL, 0, 0);
	//   SSM(SCI_SETLEXER, SCLEX_CPP, 0);
	//   SSM(SCI_SETKEYWORDS, 0, (sptr_t)"int char");
	//   SSM(SCI_STYLESETFORE, SCE_C_COMMENT, 0x008000);
	//   SSM(SCI_STYLESETFORE, SCE_C_COMMENTLINE, 0x008000);
	//   SSM(SCI_STYLESETFORE, SCE_C_NUMBER, 0x808000);
	//   SSM(SCI_STYLESETFORE, SCE_C_WORD, 0x800000);
	//   SSM(SCI_STYLESETFORE, SCE_C_STRING, 0x800080);
	//   SSM(SCI_STYLESETBOLD, SCE_C_OPERATOR, 1);
	//   SSM(SCI_INSERTTEXT, 0, (sptr_t)
	//    "int main(int argc, char **argv) {\n"
	//    "    // Start up the gnome\n"
	//    "    gnome_init(\"stest\", \"1.0\", argc, argv);\n}"
	//   );
	//
	//   gtk_widget_grab_focus(GTK_WIDGET(editor));

	return s;
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
