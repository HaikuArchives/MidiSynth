/*
 * Copyright 2021. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include "Application.h"
#include "FKeyCatcher.h"
#include "MsgConsts.h"


FKeyCatcher::FKeyCatcher(BMenu* menu)
	:
	BView("Fkey catcher", 0)
{
	chordMenu = menu;
}


FKeyCatcher::~FKeyCatcher()
{
}


void
FKeyCatcher::AttachedToWindow()
{
	SetEventMask(B_KEYBOARD_EVENTS);
	BView::AttachedToWindow();
}


void
FKeyCatcher::KeyDown(const char* bytes, int32 numBytes)
{
	if (Window()->IsActive()) {
		switch (bytes[0]) {
			case B_FUNCTION_KEY:
			{
				int32 fkey = 0;
				if (Window() != NULL && Window()->CurrentMessage() != NULL)
					fkey = Window()->CurrentMessage()->FindInt32("key");

				BMenuItem* item = chordMenu->ItemAt(fkey - 2); // F1 == 0x02
				if (item != NULL)
					item-> SetMarked(true);

				BMessenger messenger(Looper());
				BMessage message(MENU_CHORD);
				messenger.SendMessage(&message);
				break;
			}
		}
	}
}
