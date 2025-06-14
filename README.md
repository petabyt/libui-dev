# libui-dev
A fork of [libui-ng](https://github.com/libui-ng/libui-ng) - a portable UI library written in C, which uses the native UI of the operating system it runs on.  
Like wxWidgets, but in C. And much more bare-bones.

This adds a bunch of experimental features and fixes:
- [x] uiWindowSetIcon
- [x] Windows HiDPI scaling
- [x] `uiTabOnSelected`/`uiTabSetSelected` ([289](https://github.com/libui-ng/libui-ng/pull/289))
- [x] Tooltips ([266](https://github.com/libui-ng/libui-ng/pull/266)/[256](https://github.com/libui-ng/libui-ng/pull/256))
- [x] Placeholders ([224](https://github.com/libui-ng/libui-ng/pull/224))
- [x] Fix `uiWindowSetContentSize` ([261](https://github.com/libui-ng/libui-ng/pull/261))
- [x] Fix darwin menu + modal window issues ([295](https://github.com/libui-ng/libui-ng/pull/295))
- [x] Optional type safety with C11 generics (See `ui_safe.h`)
- [x] Code editing control `uiScintilla` (https://github.com/petabyt/libui-scintilla)
- [x] Merge in unfinished qt5 backend (https://github.com/mischnic/libui/tree/qt5)
- [x] OpenGLArea ([405](https://github.com/andlabs/libui/pull/405))
- [ ] Add uiDragDestination for text and file drop. https://github.com/libui-ng/libui-ng/pull/245
- [ ] [OnFilesDropped](https://github.com/matyalatte/libui-ng/commit/f3782dc426354eb09f04b8d2e1d28dbaef528ee0)
- [ ] uiScroll (WIP)
- [ ] uiImage control ([209](https://github.com/andlabs/libui/pull/209))
- [ ] uiEntry and uiEditableCombobox placeholders ([224](https://github.com/libui-ng/libui-ng/pull/224))
- [ ] uiEntryOnFinished ([290](https://github.com/andlabs/libui/pull/290))
- [ ] uiImageButton
- [ ] uiVersion API ([143](https://github.com/libui-ng/libui-ng/pull/143))
- [ ] uiDrawBitmap ([386](https://github.com/andlabs/libui/pull/386))
- [ ] Expose more system functions to make adding controls easier
- [ ] Arena allocator for uiControl handles

Potentially breaking changes:
- [x] Moved include files to include/

# Compiling
```
# Compile with meson on Linux
meson setup build
meson compile -C build

# Compile with cmake on Linux
cmake -DLIBUI_COMPILE_EXAMPLES=ON -G Ninja -B build && cmake --build build

# Compile for MinGW from Linux
cmake -DCMAKE_TOOLCHAIN_FILE=extras/mingw.cmake -G Ninja -B build && cmake --build build
```

# Credits:
- https://github.com/libui-ng/libui-ng
- https://github.com/andlabs/libui

If any libui developer wants to work on this repo let me know and I'll add you as a maintainer.
