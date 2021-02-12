#ifndef STATUS_WINDOW_H
#define STATUS_WINDOW
#include <Window.h>
#include <Rect.h>
#include <String.h>

class StatusWindow : public BWindow {
public:
	StatusWindow(const char *text, BRect);
};
#endif
