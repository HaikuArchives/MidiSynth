/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#include "Settings.h"
#include <Message.h>

#define DEFAULT_KEY_MAP				"Default"
#define DEFAULT_PATCH				"TimGM6mb.sf2"
#define DEFAULT_MAX_SYNTH_VOICES	28
#define DEFAULT_REVERB				B_REVERB_BALLROOM
#define DEFAULT_MAIN_VOLUME			1.00
#define DEFAULT_SAMPLING_RATE		48000
#define DEFAULT_KEYBOARD_OCTAVES	3
#define DEFAULT_KEYBOARD_ROWS		2
#define DEFAULT_WINDOW_POS_X		100
#define DEFAULT_WINDOW_POS_Y		100
#define DEFAULT_WINDOW_WIDTH		460
#define DEFAULT_WINDOW_HEIGHT		404
#define DEFAULT_SCOPE_VISIBLE		false
#define DEFAULT_SCOPE_WINDOW_POS_X	570
#define DEFAULT_SCOPE_WINDOW_POS_Y	100
#define DEFAULT_SCOPE_WINDOW_WIDTH	300
#define DEFAULT_SCOPE_WINDOW_HEIGHT	150


GlobalSettings::GlobalSettings()
	:
	changed(false),
	keyMap(DEFAULT_KEY_MAP),
	patch(DEFAULT_PATCH),
	mainVolume(DEFAULT_MAIN_VOLUME),
	samplingRate(DEFAULT_SAMPLING_RATE),
	maxSynthVoices(DEFAULT_MAX_SYNTH_VOICES),
	reverb(DEFAULT_REVERB),
	keyboardOctaves(DEFAULT_KEYBOARD_OCTAVES),
	keyboardRows(DEFAULT_KEYBOARD_ROWS),
	windowPos(DEFAULT_WINDOW_POS_X, DEFAULT_WINDOW_POS_Y),
	windowWidth(DEFAULT_WINDOW_WIDTH),
	windowHeight(DEFAULT_WINDOW_HEIGHT),
	scopeVisible(DEFAULT_SCOPE_VISIBLE),
	scopeWindowPos(DEFAULT_SCOPE_WINDOW_POS_X, DEFAULT_SCOPE_WINDOW_POS_Y),
	scopeWindowWidth(DEFAULT_SCOPE_WINDOW_WIDTH),
	scopeWindowHeight(DEFAULT_SCOPE_WINDOW_HEIGHT)
{
}


bool
GlobalSettings::HasChanged() const
{
	return changed;
}


const char*
GlobalSettings::GetPatch() const
{
	return patch.String();
}


void
GlobalSettings::SetPatch(const char* patch)
{
	changed |= this->patch.Compare(patch) != 0;
	this->patch = patch;
}


const char*
GlobalSettings::GetKeyMap() const
{
	return keyMap.String();
}


void
GlobalSettings::SetKeyMap(const char* map)
{
	changed |= keyMap.Compare(map) != 0;
	keyMap = map;
}


void
GlobalSettings::SetMainVolume(double volume)
{
	changed |= mainVolume != volume;
	mainVolume = volume;
}


double
GlobalSettings::GetMainVolume(void) const
{
	return mainVolume;
}


void
GlobalSettings::SetSamplingRate(int32 rate)
{
	changed |= samplingRate != rate;
	samplingRate = rate;
}


int32
GlobalSettings::GetSamplingRate(void) const
{
	return samplingRate;
}


void
GlobalSettings::SetReverb(reverb_mode reverb)
{
	changed |= this->reverb != reverb;
	this->reverb = reverb;
}


reverb_mode
GlobalSettings::GetReverb() const
{
	return reverb;
}


void
GlobalSettings::SetMaxSynthVoices(int16 voices)
{
	changed |= maxSynthVoices != voices;
	maxSynthVoices = voices;
}


int16
GlobalSettings::GetMaxSynthVoices() const
{
	return maxSynthVoices;
}


void
GlobalSettings::SetKeyboardOctaves(int16 octaves)
{
	changed |= keyboardOctaves != octaves;
	keyboardOctaves = octaves;
}


int16
GlobalSettings::GetKeyboardOctaves() const
{
	return keyboardOctaves;
}


void
GlobalSettings::SetKeyboardRows(int16 rows)
{
	changed |= keyboardRows != rows;
	keyboardRows = rows;
}


int16
GlobalSettings::GetKeyboardRows() const
{
	return keyboardRows;
}


void
GlobalSettings::SetWindowPosition(BPoint p)
{
	changed = true;
	windowPos = p;
}


BPoint
GlobalSettings::GetWindowPosition() const
{
	return windowPos;
}


void
GlobalSettings::SetWindowSize(float w, float h)
{
	changed |= (windowWidth != w) || (windowHeight != h);
	windowWidth = w;
	windowHeight = h;
}


void
GlobalSettings::GetWindowSize(float& w, float& h) const
{
	w = windowWidth;
	h = windowHeight;
}


void
GlobalSettings::SetScopeVisible(bool b)
{
	changed = true;
	scopeVisible = b;
}


bool
GlobalSettings::GetScopeVisible() const
{
	return scopeVisible;
}


void
GlobalSettings::SetScopeWindowPosition(BPoint p)
{
	changed = true;
	scopeWindowPos = p;
}


BPoint
GlobalSettings::GetScopeWindowPosition() const
{
	return scopeWindowPos;
}


void
GlobalSettings::SetScopeWindowSize(float w, float h)
{
	changed |= (scopeWindowWidth != w) || (scopeWindowHeight != h);
	scopeWindowWidth = w;
	scopeWindowHeight = h;
}


void
GlobalSettings::GetScopeWindowSize(float& w, float& h) const
{
	w = scopeWindowWidth;
	h = scopeWindowHeight;
}

// BArchivable:
GlobalSettings::GlobalSettings(BMessage* archive)
{
	changed = false;
	const char* string;
	// R4.5 compatibility:
	// in R5 FindString can handle BString objects
	if (B_OK == archive->FindString("patch", &string))
		patch = string;
	else
		patch = DEFAULT_PATCH;
	if (B_OK == archive->FindString("keyMap", &string))
		keyMap = string;
	else
		keyMap = DEFAULT_KEY_MAP;

	if (B_OK != archive->FindDouble("mainVolume", &mainVolume))
		mainVolume = DEFAULT_MAIN_VOLUME;

	if (B_OK != archive->FindInt32("samplingRate", &samplingRate))
		samplingRate = DEFAULT_SAMPLING_RATE;

	if (B_OK != archive->FindInt16("maxSynthVoices", &maxSynthVoices))
		maxSynthVoices = DEFAULT_MAX_SYNTH_VOICES;

	int16 i;

	if (B_OK == archive->FindInt16("reverb", &i))
		reverb = (reverb_mode) i;
	else
		reverb = DEFAULT_REVERB;

	if (B_OK != archive->FindInt16("octaves", &keyboardOctaves))
		keyboardOctaves = DEFAULT_KEYBOARD_OCTAVES;

	if (B_OK != archive->FindInt16("rows", &keyboardRows))
		keyboardRows = DEFAULT_KEYBOARD_ROWS;

	if (B_OK != archive->FindPoint("winPos", &windowPos))
		windowPos = BPoint(DEFAULT_WINDOW_POS_X, DEFAULT_WINDOW_POS_Y);

	if (B_OK != archive->FindFloat("winWidth", &windowWidth))
		windowWidth = DEFAULT_WINDOW_WIDTH;

	if (B_OK != archive->FindFloat("winHeight", &windowHeight))
		scopeWindowHeight = DEFAULT_SCOPE_WINDOW_HEIGHT;

	if (B_OK != archive->FindBool("scopeVisible", &scopeVisible))
		scopeVisible = DEFAULT_SCOPE_VISIBLE;

	if (B_OK != archive->FindPoint("scopePos", &scopeWindowPos))
		scopeWindowPos
			= BPoint(DEFAULT_SCOPE_WINDOW_POS_X, DEFAULT_SCOPE_WINDOW_POS_Y);

	if (B_OK != archive->FindFloat("scopeWidth", &scopeWindowWidth))
		scopeWindowWidth = DEFAULT_SCOPE_WINDOW_WIDTH;

	if (B_OK != archive->FindFloat("scopeHeight", &scopeWindowHeight))
		scopeWindowHeight = DEFAULT_SCOPE_WINDOW_HEIGHT;
}


status_t
GlobalSettings::Archive(BMessage* archive, bool deep) const
{
	archive->AddString("class", "GlobalSettings");
	archive->AddString("patch", patch.String());
	archive->AddString("keyMap", keyMap.String());
	archive->AddDouble("mainVolume", mainVolume);
	archive->AddInt32("samplingRate", samplingRate);
	archive->AddInt16("maxSynthVoices", maxSynthVoices);
	archive->AddInt16("reverb", reverb);
	archive->AddInt16("octaves", keyboardOctaves);
	archive->AddInt16("rows", keyboardRows);
	archive->AddPoint("winPos", windowPos);
	archive->AddFloat("winWidth", windowWidth);
	archive->AddFloat("winHeight", windowHeight);
	archive->AddBool("scopeVisible", scopeVisible);
	archive->AddPoint("scopePos", scopeWindowPos);
	archive->AddFloat("scopeWidth", scopeWindowWidth);
	archive->AddFloat("scopeHeight", scopeWindowHeight);
	return B_OK;
}


BArchivable*
GlobalSettings::Instantiate(BMessage* archive)
{
	if (validate_instantiation(archive, "GlobalSettings"))
		return new GlobalSettings(archive);
	else
		return NULL;
}
