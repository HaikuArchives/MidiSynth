#include "StatusWindow.h"
#include "AppDefs.h"
#include <StringView.h>

StatusWindow::StatusWindow(const char *text, BRect aRect)
    :
    BWindow(aRect, "MidiSynth Status", B_BORDERED_WINDOW,
            B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_NOT_CLOSABLE) {
  BString s("Loading Instrument Definition File: ");
  s += text;

  aRect.OffsetBy(aRect.Width() / 2, aRect.Height() / 2);
  float width = 300, height = 30;
  aRect.SetRightBottom(BPoint(aRect.left + width, aRect.top + height));
  aRect.OffsetBy(-aRect.Width() / 2, -aRect.Height() / 2);

  BStringView *view =
      new BStringView(BRect(0, 0, width, 20), "label", s.String());
  view->SetAlignment(B_ALIGN_CENTER);
  AddChild(view);

  MoveTo(aRect.left, aRect.top);
  ResizeTo(aRect.Width(), aRect.Height());
  Show();
}
