# libui-cross
Prebuilt binaries for cross-compiling software using libui.
Builds will contain a few experimental extensions.

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
