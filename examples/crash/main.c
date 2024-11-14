#include <stdio.h>
#include <ui.h>

int onClosing(uiWindow *w, void *data) {
  uiQuit();
  return 1;
}

int main(void) {
  uiInitOptions o = {0};
  const char *err = uiInit(&o);
  
  if (err != NULL) {
    fprintf(stderr, "Error initializing libui-ng: %s\n", err);
    uiFreeInitError(err);
    return 1;
  }

  uiWindow *w = uiNewWindow("Hello World!", 300, 30, 0);
  uiWindowOnClosing(w, onClosing, NULL);
  uiLabel *l = uiNewLabel("Hello, World!");
  uiWindowSetChild(w, uiControl(l));
  uiControlShow(uiControl(w));

  int width, height;
  uiWindowContentSize(w, &width, &height);
  printf("Current window size: %d x %d\n", width, height);

  uiWindowSetContentSize(w, 500, 500);
  printf("Window size set to: 500 x 500\n");

  uiMain();
  uiUninit();
  return 0;
}
