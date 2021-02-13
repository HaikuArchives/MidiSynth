/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#ifndef KEYBOARD3D_H
#define KEYBOARD3D_H

#include "Keyboard.h"
#include <GL/glu.h>
#include <GLView.h>

class Keyboard3D : public Keyboard, public BGLView
{
private:
	void gInit(void);
	void gDraw(void);
	void gReshape(int width, int height);

	float width;
	float height;

	GLenum use_stipple_mode; // GL_TRUE to use dashed lines
	GLenum use_smooth_mode; // GL_TRUE to use anti-aliased lines
	GLint linesize; // Line width
	GLint pointsize; // Point diameter

	float pntA[3], pntB[3];

public:
	Keyboard3D(View* view, BRect rect);
	void NoteOn(uchar note);
	void NoteOff(uchar note);

	virtual void AttachedToWindow(void);
	virtual void FrameResized(float newWidth, float newHeight);
	virtual void ErrorCallback(GLenum which);

	void Render(void);
};

#endif
