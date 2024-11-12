#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ui.h>

static void appendWithAttribute(uiAttributedString *attrstr, const char *what, uiAttribute *attr, uiAttribute *attr2)
{
	size_t start, end;

	start = uiAttributedStringLen(attrstr);
	end = start + strlen(what);
	uiAttributedStringAppendUnattributed(attrstr, what);
	uiAttributedStringSetAttribute(attrstr, attr, start, end);
	if (attr2 != NULL)
		uiAttributedStringSetAttribute(attrstr, attr2, start, end);
}

struct MarkdownParser {
	uiAttribute *attrs[5];
};

// Needs to be recursive
static uiAttributedString *parse_markdown(const char *string) {
	uiAttributedString *attrstr = uiNewAttributedString("");

	char buffer[512];
	int bi = 0;

	for (int i = 0; string[i] != '\0'; i++) {
		int size = 0;
		if (string[i] == '#') {
			if (bi) {
				buffer[bi] = '\0';
				uiAttributedStringAppendUnattributed(attrstr, buffer);
				bi = 0;
			}

			i++;
			size++;
			if (string[i + 1] == '#') {
				i++;
				size++;
				if (string[i + 1] == '#') {
					i++;
					size++;
				}
			}
			if (string[i] == ' ') {
				i++;
			}
			for (bi = 0; string[i] != '\n' && string[i] != '\0'; i++) {
				buffer[bi] = string[i];
				bi++;
			}
			buffer[bi] = '\0';

			uiAttribute *attr = uiNewSizeAttribute(size * 15);
			appendWithAttribute(attrstr, buffer, attr, NULL);

			if (string[i] == '\n') uiAttributedStringAppendUnattributed(attrstr, "\n");

			bi = 0;
		} else {
			buffer[bi] = string[i];
			bi++;
		}
	}

	if (bi) {
		buffer[bi] = '\0';
		uiAttributedStringAppendUnattributed(attrstr, buffer);
		bi = 0;
	}


	return attrstr;
}

static uiAttributedString *makeAttributedString(void)
{
	uiAttribute *attr, *attr2;
	uiOpenTypeFeatures *otf;

	uiAttributedString *attrstr = uiNewAttributedString(
		"Drawing strings with libui is done with the uiAttributedString and uiDrawTextLayout objects.\n"
		"uiAttributedString lets you have a variety of attributes: ");

	attr = uiNewFamilyAttribute("Courier New");
	appendWithAttribute(attrstr, "font family", attr, NULL);
	uiAttributedStringAppendUnattributed(attrstr, ", ");

	attr = uiNewSizeAttribute(18);
	appendWithAttribute(attrstr, "font size", attr, NULL);
	uiAttributedStringAppendUnattributed(attrstr, ", ");

	attr = uiNewWeightAttribute(uiTextWeightBold);
	appendWithAttribute(attrstr, "font weight", attr, NULL);
	uiAttributedStringAppendUnattributed(attrstr, ", ");

	attr = uiNewItalicAttribute(uiTextItalicItalic);
	appendWithAttribute(attrstr, "font italicness", attr, NULL);
	uiAttributedStringAppendUnattributed(attrstr, ", ");

	attr = uiNewStretchAttribute(uiTextStretchCondensed);
	appendWithAttribute(attrstr, "font stretch", attr, NULL);
	uiAttributedStringAppendUnattributed(attrstr, ", ");

	attr = uiNewColorAttribute(0.75, 0.25, 0.5, 0.75);
	appendWithAttribute(attrstr, "text color", attr, NULL);
	uiAttributedStringAppendUnattributed(attrstr, ", ");

	attr = uiNewBackgroundAttribute(0.5, 0.5, 0.25, 0.5);
	appendWithAttribute(attrstr, "text background color", attr, NULL);
	uiAttributedStringAppendUnattributed(attrstr, ", ");


	attr = uiNewUnderlineAttribute(uiUnderlineSingle);
	appendWithAttribute(attrstr, "underline style", attr, NULL);
	uiAttributedStringAppendUnattributed(attrstr, ", ");

	uiAttributedStringAppendUnattributed(attrstr, "and ");
	attr = uiNewUnderlineAttribute(uiUnderlineDouble);
	attr2 = uiNewUnderlineColorAttribute(uiUnderlineColorCustom, 1.0, 0.0, 0.5, 1.0);
	appendWithAttribute(attrstr, "underline color", attr, attr2);
	uiAttributedStringAppendUnattributed(attrstr, ". ");

	uiAttributedStringAppendUnattributed(attrstr, "Furthermore, there are attributes allowing for ");
	attr = uiNewUnderlineAttribute(uiUnderlineSuggestion);
	attr2 = uiNewUnderlineColorAttribute(uiUnderlineColorSpelling, 0, 0, 0, 0);
	appendWithAttribute(attrstr, "special underlines for indicating spelling errors", attr, attr2);
	uiAttributedStringAppendUnattributed(attrstr, " (and other types of errors) ");

	uiAttributedStringAppendUnattributed(attrstr, "and control over OpenType features such as ligatures (for instance, ");
	otf = uiNewOpenTypeFeatures();
	uiOpenTypeFeaturesAdd(otf, 'l', 'i', 'g', 'a', 0);
	attr = uiNewFeaturesAttribute(otf);
	appendWithAttribute(attrstr, "afford", attr, NULL);
	uiAttributedStringAppendUnattributed(attrstr, " vs. ");
	uiOpenTypeFeaturesAdd(otf, 'l', 'i', 'g', 'a', 1);
	attr = uiNewFeaturesAttribute(otf);
	appendWithAttribute(attrstr, "afford", attr, NULL);
	uiFreeOpenTypeFeatures(otf);
	uiAttributedStringAppendUnattributed(attrstr, ").\n");

	uiAttributedStringAppendUnattributed(attrstr, "Use the controls opposite to the text to control properties of the text.");

	return attrstr;
}

static inline uintptr_t *get_params_data(uiAreaHandler *h) {
	return ((uintptr_t *)(((uint8_t *)h) + sizeof(uiAreaHandler)));
}

static void handlerDraw(uiAreaHandler *a, uiArea *area, uiAreaDrawParams *p)
{
	uiAttributedString *attrstr = (void *)(get_params_data(a)[0]);

	uiDrawTextLayout *textLayout;
	uiFontDescriptor defaultFont;
	uiDrawTextLayoutParams params;

	params.String = attrstr;
	uiLoadControlFont(&defaultFont);
	params.DefaultFont = &defaultFont;
	params.Width = 500;
	params.Align = 0;
	textLayout = uiDrawNewTextLayout(&params);
	uiDrawText(p->Context, textLayout, 0, 0);
	uiDrawFreeTextLayout(textLayout);

	//TODO RENAME?
	uiFreeFontButtonFont(&defaultFont);
}

static void handlerMouseEvent(uiAreaHandler *a, uiArea *area, uiAreaMouseEvent *e)
{
	// do nothing
}

static void handlerMouseCrossed(uiAreaHandler *ah, uiArea *a, int left)
{
	// do nothing
}

static void handlerDragBroken(uiAreaHandler *ah, uiArea *a)
{
	// do nothing
}

static int handlerKeyEvent(uiAreaHandler *ah, uiArea *a, uiAreaKeyEvent *e)
{
	// reject all keys
	return 0;
}

static int onClosing(uiWindow *w, void *data)
{
	uiControlDestroy(uiControl(data));
	uiQuit();
	return 0;
}

static int shouldQuit(void *data)
{
	uiControlDestroy(uiControl(data));
	return 1;
}

int main(void)
{
	uiInitOptions o;
	const char *err;
	uiBox *hbox, *vbox;
	uiForm *form;

	uiWindow *mainwin;
	uiArea *area;
	uiAreaHandler *handler = calloc(1, sizeof(uiAreaHandler) + sizeof(uintptr_t));

	handler->Draw = handlerDraw;
	handler->MouseEvent = handlerMouseEvent;
	handler->MouseCrossed = handlerMouseCrossed;
	handler->DragBroken = handlerDragBroken;
	handler->KeyEvent = handlerKeyEvent;

	memset(&o, 0, sizeof (uiInitOptions));
	err = uiInit(&o);
	if (err != NULL) {
		fprintf(stderr, "error initializing ui: %s\n", err);
		uiFreeInitError(err);
		return 1;
	}

	uiOnShouldQuit(shouldQuit, mainwin);

	const char *str =
	"# HEllo, World\n"
	"Hello this is a test.\n"
	"# Hello World\n"
	"This is a test"
	;

	uiAttributedString *attrstr = parse_markdown(str);
	get_params_data(handler)[0] = (uintptr_t)attrstr;

	mainwin = uiNewWindow("libui Text-Drawing Example", 640, 480, 1);
	uiWindowSetMargined(mainwin, 1);
	uiWindowOnClosing(mainwin, onClosing, mainwin);

	hbox = uiNewHorizontalBox();
	uiBoxSetPadded(hbox, 1);
	uiWindowSetChild(mainwin, uiControl(hbox));

	vbox = uiNewVerticalBox();
	uiBoxSetPadded(vbox, 1);
	uiBoxAppend(hbox, uiControl(vbox), 0);

	area = uiNewScrollingArea(handler, 0, 1000);
	uiBoxAppend(hbox, uiControl(area), 1);

	uiControlShow(uiControl(mainwin));
	uiMain();
	uiFreeAttributedString(attrstr);
	uiUninit();
	return 0;
}
