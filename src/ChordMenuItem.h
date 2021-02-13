/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#ifndef CHORD_MENU_ITEM_H
#define CHORD_MENU_ITEM_H

#include <MenuItem.h>
#include <Message.h>
#include <String.h>

class ChordMenuItem : public BMenuItem
{
	BString chord;

public:
	ChordMenuItem(const char* chord, const char* label, BMessage* message,
		char shortcut = 0, uint32 modifiers = 0);
	const char* Chord();
};

#endif
