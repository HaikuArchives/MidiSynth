/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#include "KeyTable.h"
#include <File.h>
#include <UTF8.h>

// default key table
static uchar keyTable[] = {'q', 0, '2', 1, 'w', 2, '3', 3, 'e', 4, 'r', 5, '5',
	6, 't', 7, '6', 8, 'z', 9, '7', 10, 'u', 11,

	'i', 12, '9', 13, 'o', 14, '0', 15, 'p', 16, 60, /* '<' */ 17, 'a', 18, 'y',
	19, 's', 20, 'x', 21, 'd', 22, 'c', 23,

	'v', 24, 'g', 25, 'b', 26, 'h', 27, 'n', 28, 'm', 29, 'k', 30, 44,
	/* ',' */ 31, 'l', 32, 46, /*'.'*/ 33, 246, /* 'ö' */ 34, '-', 35,

	'Q', 36, '"', 37, 'W', 38, 36, /* '§' */ 39, 'E', 40, 'R', 41, '%', 42, 'T',
	43, '&', 44, 'Z', 45, '/', 46, 'U', 47,

	'I', 48, ')', 49, 'O', 50, '=', 51, 'P', 52, 62, /* '<' */ 53, 'A', 54, 'Y',
	55, 'S', 56, 'X', 57, 'D', 58, 'C', 59,

	'V', 60, 'G', 61, 'B', 62, 'H', 63, 'N', 64, 'M', 65, 'K', 66, 59,
	/* ';' */ 67, 'L', 68, 58, /* ':' */ 69, 214, /* 'Ö' */ 70, 95, /*'_'*/ 71,
	0, 0};
// ---------------------------------------------------------------
KeyTable::KeyTable()
{
	DefaultKeyTable();
}


void
KeyTable::DefaultKeyTable()
{
	int i;
	for (i = 0; i < KEY_TABLE_SIZE; i++)
		note[i] = 255;
	i = 0;
	while ((keyTable[i] != 0) || (keyTable[i + 1] != 0)) {
		SetKey(keyTable[i], keyTable[i + 1]);
		i += 2;
	}
}


void
KeyTable::SetKey(uchar key, uchar _note)
{
	note[key] = _note;
}

#if 0
void KeyTable::SetKey(const char *bytes, int32 numBytes, uchar note) {
uchar code[3]; int32 l = 2, state = 0;
	if (note == 255) return;
	
	if ((B_OK == convert_from_utf8(B_UNICODE_CONVERSION, bytes, &numBytes, 
		(char*)code, &l, &state)) && (l == 2) && (code[0] == 0)) {

		uchar key = code[1];
		// remove old mapping
		for (int i = 0; i < KEY_TABLE_SIZE; i++) {
			if (this->note[i] == note) {
				this->note[i] = 255; break;
			}
		}
		// set new one
		this->note[key] = note;
	}
}

uchar KeyTable::Key2Note(const char *bytes, int32 numBytes) {
uchar code[3]; int32 l = 2, state = 0;
	if ((B_OK == convert_from_utf8(B_UNICODE_CONVERSION, bytes, &numBytes, 
		(char*)code, &l, &state)) && (l == 2) && (code[0] == 0)) {
		return note[code[1]];
	}
	return 255;
}
#endif


bool
KeyTable::KeyCode2Index(int32 key, int32 modifiers, int16& index)
{
	switch (key) {
		// left & right shift, alt, alt gr, control, command, tabulator
		case 38: // tab
		case 75: // left shift
		case 92: // left control
		case 102: // left command
		case 93: // left alt
		case 95: // alt gr
		case 103: // right command
		case 104: // right menu
		case 96: // right control
			return false;
	}
	if ((modifiers & B_SHIFT_KEY) || (modifiers & B_CAPS_LOCK))
		index = key + 127;
	else if (modifiers & B_CONTROL_KEY)
		index = key + 127 + 128;
	else if (modifiers & B_OPTION_KEY)
		index = key + 127 + 128 * 2;
	else
		index = key;

	return true;
}


bool
KeyTable::SetKey(int32 key, int32 modifiers, uchar note)
{
	int16 index;
	if ((note == 255) || !KeyCode2Index(key, modifiers, index))
		return false;

	// remove old mapping
	for (int i = 0; i < KEY_TABLE_SIZE; i++)
		if (this->note[i] == note) {
			this->note[i] = 255;
			break;
		}
	// set new one
	this->note[index] = note;
	return true;
}


uchar
KeyTable::Key2Note(int32 key, int32 modifiers)
{
	int16 index;
	if (KeyCode2Index(key, modifiers, index))
		return note[index];
	else
		return 255;
}

typedef struct Data {
	uchar note;
	int16 index;
};

typedef struct WData {
	uchar note;
	int16 index;
};


bool
KeyTable::Read(BFile& file)
{
	Data data;
	for (int i = 0; i < KEY_TABLE_SIZE; i++)
		note[i] = 255;

	while (sizeof(Data) == file.Read(&data, sizeof(Data))) {
		if ((data.note >= 128) || (note[data.index] != 255))
			return false;
		note[data.index] = data.note;
	}
	return true;
}


void
KeyTable::Write(BFile& file)
{
	Data data;
	for (int i = 0; i < KEY_TABLE_SIZE; i++)
		if (note[i] != 255) {
			data.index = i;
			data.note = note[i];
			file.Write(&data, sizeof(Data));
		}
}
