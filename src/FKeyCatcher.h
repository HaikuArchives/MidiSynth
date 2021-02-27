/*
 * Copyright 2021. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef FKEYCATCHER_H
#define FKEYCATCHER_H_H

#include <Menu.h>
#include <View.h>


class FKeyCatcher : public BView {
public:
					FKeyCatcher(BMenu* menu);
					~FKeyCatcher();

	virtual void	AttachedToWindow();
	virtual	void	KeyDown(const char* bytes, int32 numBytes);

private:
	BMenu*			chordMenu;
};

#endif // FKEYCATCHER_H_H
