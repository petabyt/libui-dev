# libui-dev
A portable UI library written in C, which uses the native UI of the operating system it runs on.  
Like wxWidgets, but in C. And much more bare-bones.

This is a fork of libui-ng with some extras:
- [x] uiWindowSetIcon
- [x] Windows DPI scaling (4k)
- [ ] uiScroll (WIP)
- [ ] uiImage as a widget
- [x] Tooltips (266)
- [ ] Placeholders (224)
- [ ] uiImageButton
- [ ] OpenGLArea (405)

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

Merged PRs:
- https://github.com/libui-ng/libui-ng/pull/266 / https://github.com/libui-ng/libui-ng/pull/256

