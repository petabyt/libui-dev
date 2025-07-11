#include <stdio.h>
#include <string.h>
#include <ui.h>

static int onClosing(uiWindow *w, void *data)
{
	uiQuit();
	return 1;
}

static int onShouldQuit(void *data)
{
	uiWindow *mainwin = uiWindow(data);

	uiControlDestroy(uiControl(mainwin));
	return 1;
}

static uiControl *makeBasicControlsPage(void)
{
	uiBox *vbox;
	uiBox *hbox;
	uiGroup *group;
	uiForm *entryForm;

	vbox = uiNewVerticalBox();
	uiBoxSetPadded(vbox, 1);

	hbox = uiNewHorizontalBox();
	uiBoxSetPadded(hbox, 1);
	uiBoxAppend(vbox, uiControl(hbox), 0);

	uiBoxAppend(hbox,
		uiControl(uiNewButton("Button")),
		0);
	uiBoxAppend(hbox,
		uiControl(uiNewCheckbox("Checkbox")),
		0);

	uiBoxAppend(vbox,
		uiControl(uiNewLabel("This is a label.\nLabels can span multiple lines.")),
		0);

	uiBoxAppend(vbox,
		uiControl(uiNewHorizontalSeparator()),
		0);

	group = uiNewGroup("Entries");
	uiGroupSetMargined(group, 1);
	uiBoxAppend(vbox, uiControl(group), 1);

	entryForm = uiNewForm();
	uiFormSetPadded(entryForm, 1);
	uiGroupSetChild(group, uiControl(entryForm));

	uiEntry* entry = uiNewEntry();
	uiEntrySetPlaceholder(entry, "Type here");
	uiFormAppend(entryForm,
		"Entry",
		uiControl(entry),
		0);
	uiFormAppend(entryForm,
		"Password Entry",
		uiControl(uiNewPasswordEntry()),
		0);
	uiFormAppend(entryForm,
		"Search Entry",
		uiControl(uiNewSearchEntry()),
		0);
	uiFormAppend(entryForm,
		"Multiline Entry",
		uiControl(uiNewMultilineEntry()),
		1);
	uiFormAppend(entryForm,
		"Multiline Entry No Wrap",
		uiControl(uiNewNonWrappingMultilineEntry()),
		1);

	return uiControl(vbox);
}

// TODO make these not global
static uiSpinbox *spinbox;
static uiSlider *slider;
static uiProgressBar *pbar;

static void onSpinboxChanged(uiSpinbox *s, void *data)
{
	uiSliderSetValue(slider, uiSpinboxValue(s));
	uiProgressBarSetValue(pbar, uiSpinboxValue(s));
}

static void onSliderChanged(uiSlider *s, void *data)
{
	uiSpinboxSetValue(spinbox, uiSliderValue(s));
	uiProgressBarSetValue(pbar, uiSliderValue(s));
}

static uiControl *makeNumbersPage()
{
	uiBox *hbox;
	uiGroup *group;
	uiBox *vbox;
	uiProgressBar *ip;
	uiCombobox *cbox;
	uiEditableCombobox *ecbox;
	uiRadioButtons *rb;

	hbox = uiNewHorizontalBox();
	uiBoxSetPadded(hbox, 1);

	group = uiNewGroup("Numbers");
	uiGroupSetMargined(group, 1);
	uiBoxAppend(hbox, uiControl(group), 1);

	vbox = uiNewVerticalBox();
	uiBoxSetPadded(vbox, 1);
	uiGroupSetChild(group, uiControl(vbox));

	spinbox = uiNewSpinbox(0, 100);
	slider = uiNewSlider(0, 100);
	pbar = uiNewProgressBar();
	uiSpinboxOnChanged(spinbox, onSpinboxChanged, NULL);
	uiSliderOnChanged(slider, onSliderChanged, NULL);
	uiBoxAppend(vbox, uiControl(spinbox), 0);
	uiBoxAppend(vbox, uiControl(slider), 0);
	uiBoxAppend(vbox, uiControl(pbar), 0);

	ip = uiNewProgressBar();
	uiProgressBarSetValue(ip, -1);
	uiBoxAppend(vbox, uiControl(ip), 0);

	group = uiNewGroup("Lists");
	uiGroupSetMargined(group, 1);
	uiBoxAppend(hbox, uiControl(group), 1);

	vbox = uiNewVerticalBox();
	uiBoxSetPadded(vbox, 1);
	uiGroupSetChild(group, uiControl(vbox));

	cbox = uiNewCombobox();
	uiComboboxAppend(cbox, "Combobox Item 1");
	uiComboboxAppend(cbox, "Combobox Item 2");
	uiComboboxAppend(cbox, "Combobox Item 3");
	uiBoxAppend(vbox, uiControl(cbox), 0);

	ecbox = uiNewEditableCombobox();
	uiEditableComboboxAppend(ecbox, "Editable Item 1");
	uiEditableComboboxAppend(ecbox, "Editable Item 2");
	uiEditableComboboxAppend(ecbox, "Editable Item 3");
	uiEditableComboboxSetPlaceholder(ecbox, "Select an item");
	uiBoxAppend(vbox, uiControl(ecbox), 0);

	rb = uiNewRadioButtons();
	uiRadioButtonsAppend(rb, "Radio Button 1");
	uiRadioButtonsAppend(rb, "Radio Button 2");
	uiRadioButtonsAppend(rb, "Radio Button 3");
	uiBoxAppend(vbox, uiControl(rb), 0);

	return uiControl(hbox);
}

// TODO make this not global
static uiWindow *mainwin;

static void onOpenFileClicked(uiButton *b, void *data)
{
	uiEntry *entry = uiEntry(data);
	char *filename;

	filename = uiOpenFile(mainwin);
	if (filename == NULL) {
		uiEntrySetText(entry, "(cancelled)");
		return;
	}
	uiEntrySetText(entry, filename);
	uiFreeText(filename);
}

static void onOpenFolderClicked(uiButton *b, void *data)
{
	uiEntry *entry = uiEntry(data);
	char *filename;

	filename = uiOpenFolder(mainwin);
	if (filename == NULL) {
		uiEntrySetText(entry, "(cancelled)");
		return;
	}
	uiEntrySetText(entry, filename);
	uiFreeText(filename);
}

static void onSaveFileClicked(uiButton *b, void *data)
{
	uiEntry *entry = uiEntry(data);
	char *filename;

	filename = uiSaveFile(mainwin);
	if (filename == NULL) {
		uiEntrySetText(entry, "(cancelled)");
		return;
	}
	uiEntrySetText(entry, filename);
	uiFreeText(filename);
}

static void onMsgBoxClicked(uiButton *b, void *data)
{
	uiMsgBox(mainwin,
		"This is a normal message box.",
		"More detailed information can be shown here.");
}

static void onMsgBoxErrorClicked(uiButton *b, void *data)
{
	uiMsgBoxError(mainwin,
		"This message box describes an error.",
		"More detailed information can be shown here.");
}

static uiControl *makeDataChoosersPage(void)
{
	uiBox *hbox;
	uiBox *vbox;
	uiGrid *grid;
	uiButton *button;
	uiEntry *entry;
	uiGrid *msggrid;

	hbox = uiNewHorizontalBox();
	uiBoxSetPadded(hbox, 1);

	vbox = uiNewVerticalBox();
	uiBoxSetPadded(vbox, 1);
	uiBoxAppend(hbox, uiControl(vbox), 0);

	uiBoxAppend(vbox,
		uiControl(uiNewDatePicker()),
		0);
	uiBoxAppend(vbox,
		uiControl(uiNewTimePicker()),
		0);
	uiBoxAppend(vbox,
		uiControl(uiNewDateTimePicker()),
		0);

	uiBoxAppend(vbox,
		uiControl(uiNewFontButton()),
		0);
	uiBoxAppend(vbox,
		uiControl(uiNewColorButton()),
		0);

	uiBoxAppend(hbox,
		uiControl(uiNewVerticalSeparator()),
		0);

	vbox = uiNewVerticalBox();
	uiBoxSetPadded(vbox, 1);
	uiBoxAppend(hbox, uiControl(vbox), 1);

	grid = uiNewGrid();
	uiGridSetPadded(grid, 1);
	uiBoxAppend(vbox, uiControl(grid), 0);

	button = uiNewButton("  Open File  ");
	entry = uiNewEntry();
	uiEntrySetPlaceholder(entry, "Select a file");
	uiEntrySetReadOnly(entry, 1);
	uiButtonOnClicked(button, onOpenFileClicked, entry);
	uiGridAppend(grid, uiControl(button),
		0, 0, 1, 1,
		0, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(grid, uiControl(entry),
		1, 0, 1, 1,
		1, uiAlignFill, 0, uiAlignFill);

	button = uiNewButton("Open Folder");
	entry = uiNewEntry();
	uiEntrySetReadOnly(entry, 1);
	uiButtonOnClicked(button, onOpenFolderClicked, entry);
	uiGridAppend(grid, uiControl(button),
		0, 1, 1, 1,
		0, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(grid, uiControl(entry),
		1, 1, 1, 1,
		1, uiAlignFill, 0, uiAlignFill);

	button = uiNewButton("  Save File  ");
	entry = uiNewEntry();
	uiEntrySetReadOnly(entry, 1);
	uiButtonOnClicked(button, onSaveFileClicked, entry);
	uiGridAppend(grid, uiControl(button),
		0, 2, 1, 1,
		0, uiAlignFill, 0, uiAlignFill);
	uiGridAppend(grid, uiControl(entry),
		1, 2, 1, 1,
		1, uiAlignFill, 0, uiAlignFill);

	msggrid = uiNewGrid();
	uiGridSetPadded(msggrid, 1);
	uiGridAppend(grid, uiControl(msggrid),
		0, 3, 2, 1,
		0, uiAlignCenter, 0, uiAlignStart);

	button = uiNewButton("Message Box");
	uiButtonOnClicked(button, onMsgBoxClicked, NULL);
	uiGridAppend(msggrid, uiControl(button),
		0, 0, 1, 1,
		0, uiAlignFill, 0, uiAlignFill);
	button = uiNewButton("Error Box");
	uiButtonOnClicked(button, onMsgBoxErrorClicked, NULL);
	uiGridAppend(msggrid, uiControl(button),
		1, 0, 1, 1,
		0, uiAlignFill, 0, uiAlignFill);

	return uiControl(hbox);
}

int main(void)
{
	uiInitOptions options;
	const char *err;
	uiTab *tab;

	memset(&options, 0, sizeof (uiInitOptions));
	err = uiInit(&options);
	if (err != NULL) {
		fprintf(stderr, "error initializing libui: %s", err);
		uiFreeInitError(err);
		return 1;
	}

	mainwin = uiNewWindow("libui Control Gallery", 640, 480, 1);
	uiWindowOnClosing(mainwin, onClosing, NULL);
	uiOnShouldQuit(onShouldQuit, mainwin);

	tab = uiNewTab();
	uiWindowSetChild(mainwin, uiControl(tab));
	uiWindowSetMargined(mainwin, 1);

	uiTabAppend(tab, "Basic Controls", makeBasicControlsPage());
	uiTabSetMargined(tab, 0, 1);

	uiTabAppend(tab, "Numbers and Lists", makeNumbersPage());
	uiTabSetMargined(tab, 1, 1);

	uiTabAppend(tab, "Data Choosers", makeDataChoosersPage());
	uiTabSetMargined(tab, 2, 1);

	uiControlShow(uiControl(mainwin));
	uiMain();
	return 0;
}
