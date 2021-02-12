#ifndef STATUS_WINDOW_H
#define STATUS_WINDOW
#include <Rect.h>
#include <String.h>
#include <Window.h>

class StatusWindow : public BWindow {
public:
  StatusWindow(const char *text, BRect);
};
#endif
