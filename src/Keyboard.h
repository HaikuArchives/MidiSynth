/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <View.h>

class View;

class Keyboard {
protected:
  View *view;

public:
  Keyboard(View *view);
  virtual void NoteOn(uchar note) = 0;
  virtual void NoteOff(uchar note) = 0;
};

#include "View.h"
#endif
