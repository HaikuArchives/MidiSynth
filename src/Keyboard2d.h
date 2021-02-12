/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#ifndef KEYBOARD2D_H
#define KEYBOARD2D_H

#include "Keyboard.h"
#include <View.h>

// white width = 2,1 cm height = 14 cm
// black width = 1,2 cm height = 9 cm
#define WIDTH 21   // 21
#define HEIGHT 140 // 51

#define F_WIDTH 0.28  // 0.30
#define F_HEIGHT 0.64 // 0.70

class Keyboard2D : public BView, public Keyboard {
  int16 width, height, frameWidth, frameHeight;
  BPoint mousePosition;
  bool mouseTracking;
  uchar currentNote;
  int16 octaves, kpl, // keys per line
      wkpl,           // white keys per line
      rows, gap;

public:
  Keyboard2D(View *view, BRect rect, int16 octaves, int16 rows);
  void FrameResized(float w, float h);

  void SetOctaves(int n);
  void SetRows(int n);
  void LayoutChanged();

  void NoteOn(uchar note);
  void NoteOff(uchar note);
  void DrawKey(uchar key, uchar on, BRect &rect);
  void Draw(BRect updateRect);
  uchar PressedKey(BPoint point);
  void MouseDown(BPoint point);
  void MouseUp(BPoint point);
  void MouseMoved(BPoint point, uint32 transit, const BMessage *message);
  void MessageReceived(BMessage *msg);

  inline bool KeyCode(int32 &key, int32 &modifiers);
  void KeyDown(const char *bytes, int32 num);
  void KeyUp(const char *bytes, int32 num);
};
#endif
