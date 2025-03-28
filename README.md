# libui-dev
A fork of [libui-ng](https://github.com/libui-ng/libui-ng) - a portable UI library written in C, which uses the native UI of the operating system it runs on.  
Like wxWidgets, but in C. And much more bare-bones.

This adds a bunch of experimental features and fixes:
- [x] uiWindowSetIcon
- [x] Windows DPI scaling (4k)
- [x] `uiTabOnSelected`/`uiTabSetSelected` ([289](https://github.com/libui-ng/libui-ng/pull/289))
- [x] Tooltips ([266](https://github.com/libui-ng/libui-ng/pull/266)/[256](https://github.com/libui-ng/libui-ng/pull/256))
- [x] Placeholders ([224](https://github.com/libui-ng/libui-ng/pull/224))
- [x] Fix `uiWindowSetContentSize` ([261](https://github.com/libui-ng/libui-ng/pull/261))
- [x] Fix darwin menu + modal window issues ([295](https://github.com/libui-ng/libui-ng/pull/295))
- [x] Optional type safety with C11 generics (See `ui_safe.h`)
- [ ] uiScroll (WIP)
- [ ] uiImage control ([209](https://github.com/andlabs/libui/pull/209))
- [ ] uiEntry and uiEditableCombobox placeholders ([224](https://github.com/libui-ng/libui-ng/pull/224))
- [ ] uiEntryOnFinished ([290](https://github.com/andlabs/libui/pull/290))
- [ ] uiImageButton
- [ ] uiVersion API ([143](https://github.com/libui-ng/libui-ng/pull/143))
- [ ] uiDrawBitmap ([386](https://github.com/andlabs/libui/pull/386))
- [ ] OpenGLArea ([405](https://github.com/andlabs/libui/pull/405))
- [ ] Expose more system functions to make adding controls easier
- [ ] Code editing control `uiScintilla`
- [ ] Get Qt5 backend working (https://github.com/mischnic/libui/tree/qt5)

Potentially breaking changes:
- [x] Moved include files to include/
- [ ] Change `uiImage` API

# Compiling
```
# Compile with meson on Linux
meson setup build
meson compile -C build

# Compile with cmake on Linux
cmake -G Ninja -B build && cmake --build build

# Compile for MinGW from Linux
cmake -DCMAKE_TOOLCHAIN_FILE=extras/mingw.cmake -G Ninja -B build && cmake --build build
```

# Credits:
- https://github.com/libui-ng/libui-ng
- https://github.com/andlabs/libui

If any libui developer wants to work on this repo let me know and I'll add you as a maintainer.
