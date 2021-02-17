/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#ifndef CONNECT_APP_H
#define CONNECT_APP_H

#include "InternalSynth.h"
#include "Scope.h"
#include "Settings.h"
#include "View.h"
#include <Application.h>
#include <Directory.h>
#include <FilePanel.h>
#include <Window.h>

class AppWindow : public BWindow
{
protected:
	GlobalSettings settings;
	// int instrument[16]; // of channel
	// void ProgramChange(uchar channel, uchar instr, bool updateMenus);
	void ChangeInstrumentMenu(int group);

	void EnableMidiSynth(bool enable);

	// MidiDevices devices;
	BList synthEntries;

	BDirectory keyMapDir;
	BMenu* keyMapMenu;
	bool remapKeys;
	BFilePanel* savePanel;

	BMenu *instrumentMenu[16], *instrGroup, *groupMenu, *midiInPortMenu,
		*midiOutPortMenu, *midiSynthMenu, *patchMenu, *chordMenu;
	BMenuItem* scopeMenu;
	ScopeWindow* scopeWindow;

	CInternalSynth* midiSynth;
	bool midiSynthEnabled;

	BMenuBar* menubar;
	BView* popView;
	BMenuField* instrumentField;
	View* view;

public:
	AppWindow(BRect);

	// void DispatchMessage(BMessage *message, BHandler *target);
	void MessageReceived(BMessage* message);
	bool QuitRequested();
	void AboutRequested();
	// Settings
	void LoadSettings();
	void SaveSettings();
	// Patches
	void GetSynthEntries();
	void SetPatchesMenu(BMenu* menu);
	BEntry* FindPatch(const char* name);
	void LoadPatch();
	bool LoadPatch(BEntry* e);
	// Chords
	void LoadChords(BMenu* menu);
	void OnPatchSelected(BMessage* msg);
	void OnVolumeChanged();
	void OnChordSelected();
	// Key Mapping
	void BuildKeyMapMenu(BMenu* menu);
	void LoadKeyMap(const char* name);
	void OnRemapKeys();
	void OnSave(BMessage* msg);
	void OnKeyMapSelected(BMessage* msg);
	// Misc.
	void PopulateInstrumentMenus();
	void PopulatePortMenus();
	void OnSynthesizerEnabled();
	bool OnInstrumentChanged(BMessage* msg);
	bool OnProgramChanged(BMessage* msg);
	bool OnOctaveChanged(BMessage* msg);
	bool OnChannelChanged(BMessage* msg);
	bool OnPortChanged(BMessage* msg);
	void OnReset();
	// Reverb
	void BuildReverbMenu(BMenu* menu);
	void OnReverb(BMessage* msg);
	// SamplingRate
	void BuildSamplingRateMenu(BMenu* menu);
	void OnSamplingRate(BMessage* msg);
	// Interpolation
	void BuildInterpolationMenu(BMenu* menu);
	void OnInterpolation(BMessage* msg);
	// Max Synth Voices
	void BuildMaxSynthVoicesMenu(BMenu* menu);
	void OnMaxSynthVoices(BMessage* msg);
	// Limiter Threshhold
	void BuildLimiterThreshholdMenu(BMenu* menu);
	void OnLimiterThreshhold(BMessage* msg);
	// Keyboard
	void BuildKeyboardOctavesMenu(BMenu* menu);
	void OnKeyboardOctaves(BMessage* msg);
	void BuildKeyboardRowsMenu(BMenu* menu);
	void OnKeyboardRows(BMessage* msg);

	void OpenScopeWindow();
	void OnScope();
	void OnScopeWindowClosed();

	virtual void MenusBeginning();
	void FrameMoved(BPoint p);
	void FrameResized(float w, float h);
};

class App : public BApplication
{
public:
	AppWindow* window;
	App();
};

#define my_app ((App*) be_app)
#endif
