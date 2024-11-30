include ../../cross.mk

COMMON := scintilla/src/AutoComplete.o scintilla/src/CallTip.o scintilla/src/CaseConvert.o scintilla/src/CaseFolder.o scintilla/src/CellBuffer.o scintilla/src/ChangeHistory.o scintilla/src/CharacterCategoryMap.o scintilla/src/CharacterType.o scintilla/src/CharClassify.o scintilla/src/ContractionState.o scintilla/src/DBCS.o scintilla/src/Decoration.o scintilla/src/Document.o scintilla/src/EditModel.o scintilla/src/Editor.o scintilla/src/EditView.o scintilla/src/Geometry.o scintilla/src/Indicator.o scintilla/src/KeyMap.o scintilla/src/LineMarker.o scintilla/src/MarginView.o scintilla/src/PerLine.o scintilla/src/PositionCache.o scintilla/src/RESearch.o scintilla/src/RunStyles.o scintilla/src/Selection.o scintilla/src/Style.o scintilla/src/UndoHistory.o scintilla/src/UniConversion.o scintilla/src/UniqueString.o scintilla/src/ViewStyle.o scintilla/src/XPM.o scintilla/src/ScintillaBase.o

ifeq ($(TARGET),w)
CFLAGS := -DNDEBUG -Iscintilla/include -Iscintilla/src -Iscintilla/win32 --std=c++17 -Wpedantic -Wall -Wextra -O3 -lui

FILES := $(COMMON) scintilla/win32/HanjaDic.o scintilla/win32/PlatWin.o scintilla/win32/ScintillaWin.o
FILES += win.o

FILES := $(call convert_target,$(FILES))

test.exe: $(FILES) win.res
	$(GPP) -static $(FILES) $(LIBUI_A) $(WIN_LINK_ESSENTIALS) win.res -lstdc++ -pthread -limm32 -o test.exe

ui_scintilla_win_x86_64.a: $(FILES)
	x86_64-w64-mingw32-ar rsc ui_scintilla_win_x86_64.a $(FILES)

install: ui_scintilla_win_x86_64.a
	sudo cp ui_scintilla_win_x86_64.a $(LIB_DIR)/ui_scintilla.a
	sudo mkdir -p $(INC_DIR)/ui
	sudo cp ui_scintilla.h $(INC_DIR)/ui/ui_scintilla.h

all: $(FILES)

endif

ifeq ($(TARGET),l)

CFLAGS := -DGTK -DNDEBUG -Iscintilla/include -Iscintilla/src -Wpedantic -Wall -Wextra -O3 -lui `pkg-config --cflags --libs gtk+-3.0`

FILES += $(COMMON) scintilla/gtk/PlatGTK.o scintilla/gtk/ScintillaGTKAccessible.o scintilla/gtk/ScintillaGTK.o scintilla/gtk/scintilla-marshal.o
FILES += gtk.o

FILES := $(call convert_target,$(FILES))

ui_scintilla_gtk_x86_64.a: $(FILES)
	ar rsc ui_scintilla_gtk_x86_64.a $(FILES)

install: ui_scintilla_gtk_x86_64.a
	sudo cp ui_scintilla_gtk_x86_64.a $(LIB_DIR)/ui_scintilla.a
	sudo mkdir -p $(INC_DIR)/ui
	sudo cp ui_scintilla.h $(INC_DIR)/ui

endif

clean:
	make TARGET=w clean_files
	make TARGET=l clean_files

clean_files:
	rm -rf *.exe $(FILES) *.a *.o *.res *.d
