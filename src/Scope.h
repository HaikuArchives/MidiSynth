#ifndef SCOPE_H
#define SCOPE_H

#include <View.h>
#include <Window.h>
#include "Settings.h"

class Scope : public BView {
	BBitmap *mBitmap;
	BView *mOffscreenView;
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

class ScopeWindow : public BWindow {
	GlobalSettings *mSettings;
	BLooper *mLooper;
public:
	ScopeWindow(BLooper *looper, GlobalSettings *settings, BRect rect);
	virtual void Quit();
	void FrameMoved(BPoint p);
	void FrameResized(float w, float h);
};

#endif