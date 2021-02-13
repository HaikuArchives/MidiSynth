/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <Archivable.h>
#include <Point.h>
#include <String.h>
#include <Synth.h>

// Renamed class Settings to GlobalSettings because of Linker warning
// in PPC Crosscompiler (libtracker)
class _EXPORT GlobalSettings : public BArchivable
{
	bool changed;
	BString keyMap, patch;
	double mainVolume;
	int32 samplingRate;
	int16 maxSynthVoices;
	reverb_mode reverb;
#ifndef __HAIKU__
	interpolation_mode interpolation;
	int16 limiterThreshhold;
#endif
	int16 keyboardOctaves, keyboardRows;
	BPoint windowPos;
	float windowWidth, windowHeight;

	BPoint scopeWindowPos;
	float scopeWindowWidth, scopeWindowHeight;

public:
	GlobalSettings();
	bool HasChanged() const;

	const char* GetPatch() const;
	const char* GetKeyMap() const;

	void SetPatch(const char* patch);
	void SetKeyMap(const char* map);

	void SetMainVolume(double volume);
	double GetMainVolume(void) const;

	void SetSamplingRate(int32 rate);
	int32 GetSamplingRate(void) const;

	void SetReverb(reverb_mode reverb);
	reverb_mode GetReverb() const;

	void SetMaxSynthVoices(int16 voices);
	int16 GetMaxSynthVoices() const;

#ifndef __HAIKU__
	void SetInterpolation(interpolation_mode im);
	interpolation_mode GetInterpolation() const;

	void SetLimiterThreshhold(int16 threshhold);
	int16 GetLimiterThreshhold() const;
#endif

	void SetKeyboardOctaves(int16 octaves);
	int16 GetKeyboardOctaves() const;

	void SetKeyboardRows(int16 rows);
	int16 GetKeyboardRows() const;

	void SetWindowPosition(BPoint p);
	BPoint GetWindowPosition() const;

	void SetWindowSize(float w, float h);
	void GetWindowSize(float& w, float& h) const;

	void SetScopeWindowPosition(BPoint p);
	BPoint GetScopeWindowPosition() const;

	void SetScopeWindowSize(float w, float h);
	void GetScopeWindowSize(float& w, float& h) const;

	// BArchivable:
	GlobalSettings(BMessage* archive);
	status_t Archive(BMessage* archive, bool deep = true) const;
	static BArchivable* Instantiate(BMessage* archive);
};

#endif
