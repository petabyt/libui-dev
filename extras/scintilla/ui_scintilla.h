#ifdef __cplusplus
extern "C" {
#endif
 
typedef struct uiScintilla uiScintilla;
_UI_EXTERN void uiScintillaGetRange(uiScintilla *s, int start, int end, char *text);
_UI_EXTERN void uiScintillaSetText(uiScintilla *s, const char *text);
_UI_EXTERN void uiScintillaAppend(uiScintilla *s, const char *text);
_UI_EXTERN int uiScintillaGetLength(uiScintilla *s);
_UI_EXTERN uiScintilla *uiNewScintilla();
_UI_EXTERN char *uiScintillaText(uiScintilla *s);

#ifdef __cplusplus
}
#endif
