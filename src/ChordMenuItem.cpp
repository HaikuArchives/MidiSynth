/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#include "ChordMenuItem.h"


ChordMenuItem::ChordMenuItem(const char* chord, const char* label,
	BMessage* message, char shortcut, uint32 modifiers)
	:
	BMenuItem(label, message, shortcut, modifiers)
{
	this->chord = chord;
}


const char*
ChordMenuItem::Chord()
{
	return chord.String();
}
