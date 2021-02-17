/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#include "Scope.h"
#include "MsgConsts.h"

#include <Bitmap.h>
#include <Catalog.h>
#include <MessageQueue.h>
#include <Synth.h>


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Scope"

Scope::Scope(BRect rect)
	:
	BView(
		rect, NULL, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_PULSE_NEEDED)
{
	SetViewColor(B_TRANSPARENT_COLOR);

	mWidth = (int) rect.Width();
	mHeight = (int) rect.Height();
	mSampleCount = mWidth;
	mLeft = new int16[mSampleCount];
	mRight = new int16[mSampleCount];

	mBitmap
		= new BBitmap(BRect(0, 0, mWidth, mHeight), B_RGB_16_BIT, true, true);
	if (mBitmap->Lock()) {
		mOffscreenView
			= new BView(mBitmap->Bounds(), "", B_FOLLOW_NONE, B_WILL_DRAW);
		mOffscreenView->SetHighColor(0, 0, 0, 0);

		mBitmap->AddChild(mOffscreenView);
		mOffscreenView->FillRect(mBitmap->Bounds());
		mOffscreenView->Sync();
		mBitmap->Unlock();
	}
}


void
Scope::Draw(BRect updateRect)
{
	DrawBitmap(mBitmap, BPoint(0, 0));
}


void
Scope::DrawSample()
{
	int32 size = 0;
	if (be_synth)
		size = be_synth->GetAudio(mLeft, mRight, mSampleCount);

	if (mBitmap->Lock()) {
		mOffscreenView->SetHighColor(0, 0, 0, 0);
		mOffscreenView->FillRect(mBitmap->Bounds());
		if (size > 0) {
			mOffscreenView->SetHighColor(255, 0, 0, 0);
			mOffscreenView->SetLowColor(0, 255, 0, 0);
#define N 16
			int32 x, y, sx = 0, f = (mHeight << N) / 65535, dy = mHeight / 2;
			for (int32 i = 0; i < mWidth; i++) {
				x = sx / mWidth;
				y = ((mLeft[x] * f) >> N) + dy;
				mOffscreenView->FillRect(BRect(i, y, i, y));
				y = ((mRight[x] * f) >> N) + dy;
				mOffscreenView->FillRect(BRect(i, y, i, y), B_SOLID_LOW);
				sx += size;
			}
		}
		mOffscreenView->Sync();
		mBitmap->Unlock();
	}
}

#define PULSE_RATE 1 * 100000


void
Scope::AttachedToWindow(void)
{
	Window()->SetPulseRate(PULSE_RATE);
}


void
Scope::DetachedFromWindow(void)
{
	Window()->SetPulseRate(0);
	delete mLeft;
	delete mRight;

	if (mBitmap->Lock()) {
		mBitmap->RemoveChild(mOffscreenView);
		delete mOffscreenView;
		mBitmap->Unlock();
	}
	delete mBitmap;
}


void
Scope::Pulse()
{
	DrawSample();
	BMessage* m;
	BMessageQueue* mq = Window()->MessageQueue();
	int i = 0;
	while ((m = mq->FindMessage(B_PULSE, 0)) != NULL)
		mq->RemoveMessage(m), i++;
	Invalidate();
}

// ScopeWindow
ScopeWindow::ScopeWindow(BLooper* looper, GlobalSettings* settings, BRect rect)
	:
	BWindow(rect, B_TRANSLATE("Scope"), B_FLOATING_WINDOW_LOOK,
		B_NORMAL_WINDOW_FEEL, NULL),
	mSettings(settings),
	mLooper(looper)
{
	AddChild(new Scope(BRect(0, 0, rect.Width(), rect.Height())));
	Show();
}


void
ScopeWindow::Quit()
{
	mLooper->PostMessage(MSG_SCOPE_CLOSED_NOTIFY);
	BWindow::Quit();
}


void
ScopeWindow::FrameMoved(BPoint p)
{
	mSettings->SetScopeWindowPosition(p);
}


void
ScopeWindow::FrameResized(float w, float h)
{
	mSettings->SetScopeWindowSize(w, h);
}
