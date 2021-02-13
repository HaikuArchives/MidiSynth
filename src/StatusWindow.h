/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#ifndef STATUS_WINDOW_H
#define STATUS_WINDOW
#include <Rect.h>
#include <String.h>
#include <Window.h>

class StatusWindow : public BWindow
{
public:
	StatusWindow(const char* text, BRect);
};
#endif
