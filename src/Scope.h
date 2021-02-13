/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#ifndef SCOPE_H
#define SCOPE_H

#include "Settings.h"
#include <View.h>
#include <Window.h>

class Scope : public BView
{
	BBitmap* mBitmap;
	BView* mOffscreenView;
	int mWidth, mHeight;
	int32 mSampleCount;
	int16 *mLeft, *mRight;

public:
	Scope(BRect rect);

	void Draw(BRect updateRect);
	void DrawSample();

	void Pulse();
	void AttachedToWindow(void);
	void DetachedFromWindow(void);
};

class ScopeWindow : public BWindow
{
	GlobalSettings* mSettings;
	BLooper* mLooper;

public:
	ScopeWindow(BLooper* looper, GlobalSettings* settings, BRect rect);
	virtual void Quit();
	void FrameMoved(BPoint p);
	void FrameResized(float w, float h);
};

#endif
