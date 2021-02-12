/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#include "Keyboard2d.h"
#include "MsgConsts.h"
#include <Message.h>
#include <Window.h>
#include <math.h>

Keyboard2D::Keyboard2D(View *view, BRect rect, int16 octaves, int16 rows)
    :
    BView(rect, NULL, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM,
          B_WILL_DRAW | B_NAVIGABLE | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE),
    Keyboard(view),
    octaves(octaves),
    rows(rows) {
  FrameResized(rect.Width(), rect.Height());
  SetViewColor(B_TRANSPARENT_COLOR);
  mouseTracking = false;

  SetLowColor(255, 255, 255, 255);
}

void Keyboard2D::FrameResized(float width, float height) {
  LayoutChanged();
  // ResizeTo(frameWidth, frameHeight);
}

void Keyboard2D::SetOctaves(int n) { octaves = n; }

void Keyboard2D::SetRows(int n) { rows = n; }

void Keyboard2D::LayoutChanged() {
  int32 width = Bounds().IntegerWidth() + 1,
        height = Bounds().IntegerHeight() + 1;
  int16 w, h;

  kpl = octaves * 12;
  wkpl = octaves * 7;

  if (rows == 1) {
    w = width / wkpl;
    h = height;
  } else {
    w = width / wkpl;
    h = height / 2;
  }

  if (w > h * 0.15) {
    this->width = (int16)(h * 0.15);
    this->height = (int16)(this->width / 0.15);
  } else {
    this->width = w;
    this->height = (int16)(w / 0.15);
  }

  if (rows == 1) {
    gap = 1000;
    frameWidth = this->width * wkpl;
    frameHeight = this->height;
  } else {
    gap = (int16)(height - 2 * this->height);
    frameWidth = this->width * wkpl;
    frameHeight = (int16)height;
  }
}

void Keyboard2D::DrawKey(uchar key, uchar on, BRect &rect) {
  BRect r;
  float dxW = rect.Width() * F_WIDTH;
  float dxB = dxW + 1;
  float dyW = rect.Height() * F_HEIGHT;
  float dyB = dyW + 1;
  // 0..released, 1..pressed, 2..chord
  static rgb_color whiteKeyColor[3] = {
      {255, 255, 255, 0}, {255, 255, 0, 0}, {225, 225, 255, 0}};
  static rgb_color blackKeyColor[3] = {
      {0, 0, 0, 0}, {255, 255, 0, 0}, {128, 128, 255, 0}};
  static rgb_color outlineColor[2] = {{128, 128, 128, 0}, {192, 192, 192, 0}};

  BPoint p;

  if (on) {
    on = (on == CHORD) ? 2 : 1;
  }
  switch (key) {
  case 0: // white key before black key
  case 5:
    SetHighColor(whiteKeyColor[on]);
    r.Set(rect.left + 1, rect.top + 1, rect.right - dxB - 1, rect.bottom - 1);
    FillRect(r);

    r.Set(rect.right - dxB, rect.top + dyB + 1, rect.right - 1,
          rect.bottom - 1);
    FillRect(r);

    BeginLineArray(6);
    AddLine(BPoint(rect.left, rect.top), p = BPoint(rect.right - dxB, rect.top),
            outlineColor[0]);
    AddLine(p, p = BPoint(rect.right - dxB, rect.top + dyB), outlineColor[0]);
    AddLine(p, p = BPoint(rect.right, rect.top + dyB), outlineColor[0]);
    AddLine(p, p = BPoint(rect.right, rect.bottom), outlineColor[0]);
    AddLine(p, p = BPoint(rect.left, rect.bottom), outlineColor[1]);
    AddLine(p, p = BPoint(rect.left, rect.top), outlineColor[1]);
    EndLineArray();
    rect.OffsetBy(rect.Width(), 0);
    break;
  case 1: // black keys
  case 3:
  case 6:
  case 8:
  case 10:
    r.Set(rect.left - dxW + 1, rect.top + 1, rect.left + dxW - 1,
          rect.top + dyW - 1);
    SetHighColor(blackKeyColor[on]);
    FillRect(r);

    SetHighColor(0, 0, 0, 0);
    BeginLineArray(4);
    AddLine(BPoint(rect.left - dxW, rect.top),
            p = BPoint(rect.left + dxW, rect.top), outlineColor[0]);
    AddLine(p, p = BPoint(rect.left + dxW, rect.top + dyW), outlineColor[0]);
    AddLine(p, p = BPoint(rect.left - dxW, rect.top + dyW), outlineColor[1]);
    AddLine(p, p = BPoint(rect.left - dxW, rect.top), outlineColor[1]);
    EndLineArray();
    rect.OffsetBy(1, 0);
    return;
  case 2: // white key between black keys
  case 7:
  case 9:
    SetHighColor(whiteKeyColor[on]);
    r.Set(rect.left + dxB, rect.top + 1, rect.right - dxB - 1, rect.top + dyB);
    FillRect(r);

    r.Set(rect.left + 1, rect.top + dyB + 1, rect.right - 1, rect.bottom - 1);
    FillRect(r);

    BeginLineArray(8);
    AddLine(BPoint(rect.left + dxW, rect.top),
            p = BPoint(rect.right - dxB, rect.top), outlineColor[0]);
    AddLine(p, p = BPoint(rect.right - dxB, rect.top + dyB), outlineColor[0]);
    AddLine(p, p = BPoint(rect.right, rect.top + dyB), outlineColor[0]);
    AddLine(p, p = BPoint(rect.right, rect.bottom), outlineColor[0]);
    AddLine(p, p = BPoint(rect.left, rect.bottom), outlineColor[1]);
    AddLine(p, p = BPoint(rect.left, rect.top + dyB), outlineColor[1]);
    AddLine(p, p = BPoint(rect.left + dxW, rect.top + dyB), outlineColor[1]);
    AddLine(p, p = BPoint(rect.left + dxW, rect.top), outlineColor[1]);
    EndLineArray();
    rect.OffsetBy(rect.Width(), 0);
    break;
  case 4: // white key after black key
  case 11:
    SetHighColor(whiteKeyColor[on]);
    r.Set(rect.left + dxB, rect.top + 1, rect.right - 1, rect.bottom - 1);
    FillRect(r);

    r.Set(rect.left + 1, rect.top + dyB + 1, rect.left + dxB, rect.bottom - 1);
    FillRect(r);

    BeginLineArray(6);
    AddLine(BPoint(rect.left + dxW, rect.top), p = BPoint(rect.right, rect.top),
            outlineColor[0]);
    AddLine(p, p = BPoint(rect.right, rect.bottom), outlineColor[0]);
    AddLine(p, p = BPoint(rect.left, rect.bottom), outlineColor[1]);
    AddLine(p, p = BPoint(rect.left, rect.top + dyB), outlineColor[1]);
    AddLine(p, p = BPoint(rect.left + dxW, rect.top + dyB), outlineColor[1]);
    AddLine(p, p = BPoint(rect.left + dxW, rect.top), outlineColor[1]);
    EndLineArray();
    rect.OffsetBy(rect.Width() + 1, 0);
    break;
  }
}

void Keyboard2D::Draw(BRect updateRect) {
  BRect rect = Bounds();
  int i;
  int note = view->GetNoteOffset();

  rect.Set(0, 0, width - 1, height - 1);
  for (i = 0; i < kpl; i++) {
    DrawKey(i % 12, view->NotePlaying(note++), rect);
  }

  if (rect.left <= updateRect.right) {
    BRect r(rect.left, rect.top, updateRect.right, updateRect.bottom);
    FillRect(r, B_SOLID_LOW);
  }

  if (rows == 1) {
    if (rect.bottom <= updateRect.bottom) {
      BRect r(updateRect.left, rect.bottom + 1, updateRect.right,
              updateRect.bottom);
      FillRect(r, B_SOLID_LOW);
    }
  } else { // rows == 2
    if (gap > 0) {
      rect.Set(updateRect.left, height, updateRect.right, height + gap - 1);
      FillRect(rect, B_SOLID_LOW);
    }
    rect.Set(0, height + gap, width - 1, height + gap + height - 1);
    int16 to = kpl * 2;
    for (; i < to; i++)
      DrawKey(i % 12, view->NotePlaying(note++), rect);
  }
}

void Keyboard2D::NoteOn(uchar note) {
  Window()->PostMessage(MSG_INVALIDATE, this);
}

void Keyboard2D::NoteOff(uchar note) {
  Window()->PostMessage(MSG_INVALIDATE, this);
}

bool Keyboard2D::KeyCode(int32 &key, int32 &modifiers) {
  BMessage *msg = Window()->CurrentMessage();
  return (B_OK == msg->FindInt32("key", &key)) &&
         (B_OK == msg->FindInt32("modifiers", &modifiers));
}

void Keyboard2D::KeyDown(const char *bytes, int32 num) {
  int32 key, modifiers;
  if (KeyCode(key, modifiers))
    view->KeyDown(key, modifiers);
}

void Keyboard2D::KeyUp(const char *bytes, int32 num) {
  int32 key, modifiers;
  if (KeyCode(key, modifiers))
    view->KeyUp(key, modifiers);
}

void Keyboard2D::MessageReceived(BMessage *msg) {
  if (msg->what == MSG_INVALIDATE) {
    Invalidate();
  } else
    BView::MessageReceived(msg);
}

uchar Keyboard2D::PressedKey(BPoint point) {
  int note = 0;
  int x = (int)point.x, y = (int)point.y;
  if (x >= frameWidth)
    return 255;

  if (y >= height) {
    if (y < height + gap)
      return 255;
    else {
      note = kpl;
      y -= height + 9;
    }
  }
  int octave = x / (width * 7);
  if (octave > octaves)
    return 255;
  note += octave * 12;

  x %= width * 7;
  int key;
  if (y < height * F_HEIGHT) {
    int offset = ((int)(width - width * F_WIDTH / 2 + x)) % width;
    if (offset > width * (1 - 2 * F_WIDTH)) {
      key = (int)((width - width * F_WIDTH / 2 + x) / width);
      switch (key) {
      case 1:
      case 2:
        return (uchar)(2 * key - 1 + note);
      case 4:
      case 5:
      case 6:
        return (uchar)(2 * key - 2 + note);
      }
    }
  }
  key = x / width;
  if (key > 6)
    key = 6;
  return note + ((key > 2) ? key * 2 - 1 : key * 2);
}

void Keyboard2D::MouseDown(BPoint point) {
  MakeFocus();
  uchar note = PressedKey(point);
  mousePosition = point;
  mouseTracking = true;
  currentNote = note;
  if (note == 255)
    return;
  view->NoteOn(note, MOUSE, true);
}

void Keyboard2D::MouseUp(BPoint point) {
  uchar note = PressedKey(point);
  mouseTracking = false;
  if (note == 255)
    return;
  view->NoteOff(note, MOUSE, true);
}

void Keyboard2D::MouseMoved(BPoint point, uint32 transit,
                            const BMessage *message) {
  if (transit == B_ENTERED_VIEW) {
    BPoint dummy;
    uint32 buttons;
    GetMouse(&dummy, &buttons);
    if (buttons)
      MouseDown(point);
  } else if ((transit == B_INSIDE_VIEW) && (mouseTracking)) {
    uchar note = PressedKey(point);
    if (note != currentNote) {
      MouseUp(mousePosition);
      MouseDown(point);
    }
  } else if (transit == B_EXITED_VIEW) {
    MouseUp(mousePosition);
  }
}
