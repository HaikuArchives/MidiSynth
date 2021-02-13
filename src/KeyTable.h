/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#ifndef KEYTABLE
#define KEYTABLE
#include <File.h>
#include <SupportDefs.h>

#define KEY_TABLE_SIZE 128 * 4

class KeyTable
{
	uchar note[KEY_TABLE_SIZE];

public:
	KeyTable();
	void SetKey(uint8 key, uchar note);
	//	void SetKey(const char *bytes, int32 numBytes, uchar note);
	//	uchar Key2Note(const char *bytes, int32 numBytes);
	inline bool KeyCode2Index(int32 key, int32 modifiers, int16& index);
	bool SetKey(int32 key, int32 modifiers, uchar note);
	uchar Key2Note(int32 key, int32 modifiers);
	void DefaultKeyTable();
	bool Read(BFile& file);
	void Write(BFile& file);
};

#endif
