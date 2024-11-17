# libui-dev
A fork of LibUI-ng - a portable UI library written in C, which uses the native UI of the operating system it runs on.  
Like wxWidgets, but in C. And much more bare-bones.

This adds a bunch of experimental features and fixes:
- [x] uiWindowSetIcon
- [x] Windows DPI scaling (4k)
- [ ] uiScroll (WIP)
- [ ] uiImage as a widget
- [x] `uiTabOnSelected`/`uiTabSetSelected` (289)
- [x] Tooltips (266)
- [x] Placeholders (224)
- [x] Fix `uiWindowSetContentSize` ([261](https://github.com/libui-ng/libui-ng/pull/261))
- [ ] uiImageButton
- [ ] OpenGLArea (405)
- [ ] Type safety with C11 generics

Significant changes:
- [x] Moved include files to include/
- [ ] Change `uiImage` API

# Build system
Meson build system is preserved, but a make-based system is added for cross-compilation.
```
# Compile for MinGW from Linux
make TARGET=w libui_win64.a install

# Compile for linux from linux
make TARGET=l libui.so install

# Compile for Mac from Darling/Mac
make TARGET=m libui.dylib
```

- [x] X86_64 Linux (.so, .AppImage)
- [x] X86_64 Windows (.a, .exe)
- [ ] X86_64 MacOS (.dylib, .app)
- [ ] ARM64 Linux
- [ ] ARM64 MacOS

# Credits:
- https://github.com/libui-ng/libui-ng
- https://github.com/andlabs/libui
### Merged PRs:
- https://github.com/libui-ng/libui-ng/pull/266 / https://github.com/libui-ng/libui-ng/pull/256
- https://github.com/libui-ng/libui-ng/pull/224
- https://github.com/libui-ng/libui-ng/pull/289
