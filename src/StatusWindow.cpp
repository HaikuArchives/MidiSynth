/*
 * Copyright 2000-2021. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	2000-2013, Michael Pfeiffer et al.
 *  2021 Humdinger, humdingerb@gmail.com
 *
 */

#include "StatusWindow.h"
#include "AppDefs.h"

#include <Catalog.h>
#include <LayoutBuilder.h>
#include <StringView.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "StatusWindow"

StatusWindow::StatusWindow(const char* text, BRect aRect)
	:
	BWindow(aRect, B_TRANSLATE("MidiSynth status"), B_BORDERED_WINDOW,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_NOT_CLOSABLE)
{
	BString s(B_TRANSLATE("Loading SoundFont: %file%"));
	s.ReplaceFirst ("%file%", text);

	BStringView* view = new BStringView("label", s.String());
	view->SetAlignment(B_ALIGN_CENTER);
	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.Add(view)
		.End();

	CenterIn(aRect);
	Show();
}
