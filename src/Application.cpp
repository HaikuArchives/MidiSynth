/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	2000-2013, Michael Pfeiffer et al.
 *  2021 Humdinger, humdingerb@gmail.com
 *
 */

#include "Application.h"
#include "ChordMenuItem.h"
#include "ChordParser.h"
#include "InternalSynth.h"
#include "Keyboard2d.h"
#include <LayoutBuilder.h>
#include "MsgConsts.h"
#include "Scope.h"
#include "StatusWindow.h"
#include <MidiRoster.h>
#include <Roster.h>
#include <StorageKit.h>
#include <SupportKit.h>
#include <ctype.h>

#define INSTR_GROUPS 16
#define INSTRUMENTS 128

static const char settingsFolder[] = "MidiSynth";
static const char settingsFilename[] = "MidiSynth";

static char const* instruments[128] = {
	// Pianos (0-7)
	"Acoustic Grand", "Bright Grand", "Electric Grand", "Honky Tonk",
	"Electric Piano", "Electric Piano 2", "Harpsichord", "Clavichord",
	// Tuned Idiophones (8-15)
	"Celesta", "Glockenspiel", "Music Box", "Vibraphone", "Marimba",
	"Xylophone", "Tubular Bells", "Dulcimer",
	// Organs (16-23)
	"Drawbar Organ", "Percussive Organ", "Rock Organ", "Church Organ",
	"Reed Organ", "Accordion", "Harmonica", "Tango Accordion",
	// Guitars (24-31)
	"Acoustic Guitar Nylon", "Acoustic Guitar Steel", "Electric Guitar Jazz",
	"Electric Guitar Clean", "Electric Guitar Muted", "Overdriven Guitar",
	"Distortion Guitar", "Guitar Harmonics",
	// Basses (32-39)
	"Acoustic Bass", "Electric Bass Finger", "Electric Bass Pick",
	"Fretless Bass", "Slap Bass 1", "Slap Bass 2", "Synth Bass 1",
	"Synth Bass 2",
	// Strings And Timpani (40-47)
	"Violin", "Viola", "Cello", "Contrabass", "Tremolo Strings",
	"Pizzicato Strings", "Orchestral Strings", "Timpani",
	// Ensemble Strings And Voices (48-55)
	"String Ensemble 1", "String Ensemble 2", "Synth Strings 1",
	"Synth Strings 2", "Voice Aah", "Voice Ooh", "Synth Voice", "Orchestra Hit",
	// Brass (56-63)
	"Trumpet", "Trombone", "Tuba", "Muted Trumpet", "French Horn",
	"Brass Section", "Synth Brass 1", "Synth Brass 2",
	// Reeds (64-71)
	"Soprano Sax", "Alto Sax", "Tenor Sax", "Baritone Sax", "Oboe",
	"English Horn", "Bassoon", "Clarinet",
	// Pipes (72-79)
	"Piccolo", "Flute", "Recorder", "Pan Flute", "Blown Bottle", "Shakuhachi",
	"Whistle", "Ocarina",
	// Synth Leads (80-87)
	"Square Wave", "Sawtooth Wave", "Calliope", "Chiff", "Charang", "Voice",
	"Fifths", "Bass Lead",
	// Synth Pads (88-95)
	"New Age", "Warm", "Polysynth", "Choir", "Bowed", "Metallic", "Halo",
	"Sweep",
	// Musical Effects (96-103)
	"Fx1", "Fx2", "Fx3", "Fx4", "Fx5", "Fx6", "Fx7", "Fx8",
	// Ethnic (104-111)
	"Sitar", "Banjo", "Shamisen", "Koto", "Kalimba", "Bagpipe", "Fiddle",
	"Shanai",
	// Percussion (112-119)
	"Tinkle Bell", "Agogo", "Steel Drums", "Woodblock", "Taiko Drums",
	"Melodic Tom", "Synth Drum", "Reverse Cymbal",
	// Sound Effects (120-127)
	"Fret Noise", "Breath Noise", "Seashore", "Bird Tweet", "Telephone",
	"Helicopter", "Applause", "Gunshot"};

static char const* instrGroups[INSTR_GROUPS] = {"Pianos", "Tuned Idiophones",
	"Organs", "Guitars", "Basses", "Strings and Timpani",
	"Ensemble Strings and Voices", "Brass", "Reeds", "Pipes", "Synth Leads",
	"Synth Pads", "Musical Effects", "Ethnic", "Percussion", "Sound Effects"};

static struct {
	uchar from, to;
} instrIndizes[INSTR_GROUPS] = {{0, 7}, {8, 15}, {16, 23}, {24, 31}, {32, 39},
	{40, 47}, {48, 55}, {56, 63}, {64, 71}, {72, 79}, {80, 87}, {88, 95},
	{96, 103}, {104, 111}, {112, 119}, {120, 127}};

// function prototypes
static BMessage* NewMessage(int32 what, int32 data);
static void AddMenuItem(
	BMenu* menu, const char* name, int32 what, int32 data, int32 current);


static BMessage*
NewMessage(int32 what, int32 data)
{
	BMessage* m = new BMessage(what);
	m->AddInt32("data", data);
	return m;
}


static void
AddMenuItem(
	BMenu* menu, const char* name, int32 what, int32 data, int32 current)
{
	BMenuItem* item = new BMenuItem(name, NewMessage(what, data));
	menu->AddItem(item);
	if (data == current)
		item->SetMarked(true);
}


AppWindow::AppWindow(BRect aRect)
	:
	BWindow(aRect, "MidiSynth", B_TITLED_WINDOW,
		B_ASYNCHRONOUS_CONTROLS | B_NOT_ZOOMABLE)
{
	SetSizeLimits(460, 10000, 155, 10000);
	midiSynth = new CInternalSynth("MidiSynth");
	midiSynth->Init();
	// Save Panel
	savePanel = new BFilePanel(B_SAVE_PANEL, new BMessenger(this));

	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
		path.Append(settingsFolder);
		BDirectory dir(path.Path());
		if (B_OK != keyMapDir.SetTo(&dir, "KeyMappings"))
			keyMapDir = dir;
		savePanel->SetPanelDirectory(&keyMapDir);
	}

	LoadSettings();

	// Place window
	float w, h;
	settings.GetWindowSize(w, h);
	MoveTo(settings.GetWindowPosition());
	ResizeTo(w, h);

	// add menu bar
	menubar = new BMenuBar("menu_bar");
	midiSynthEnabled = false;
	remapKeys = false;

	BMenu *menu, *submenu;
	BMenuItem* item;
	int i, j;
	char string[B_OS_NAME_LENGTH];

	GetSynthEntries();

	midiSynthMenu = menu = new BMenu("MidiSynth");
	// Reset
	menu->AddItem(new BMenuItem("Reset", new BMessage(MENU_RESET), 'R'));
	menu->AddItem(
		scopeMenu = new BMenuItem("Scope", new BMessage(MENU_SCOPE), 'V'));
	menu->AddSeparatorItem();
	// Synthesizer
	menu->AddItem(item = new BMenuItem("Disable Synthesizer",
		new BMessage(MENU_SYNTH_ENABLED), 'S'));
	patchMenu = new BMenu("Patches");
	patchMenu->SetRadioMode(true);
	SetPatchesMenu(patchMenu);
	menu->AddItem(patchMenu);
	// Reverb
	BuildReverbMenu(submenu = new BMenu("Reverb"));
	menu->AddItem(submenu);
	// Sampling Rate
	BuildSamplingRateMenu(submenu = new BMenu("Sampling Rate"));
	menu->AddItem(submenu);
	// Max Synth Voices
	BuildMaxSynthVoicesMenu(submenu = new BMenu("Max Synth Voices"));
	menu->AddItem(submenu);
	menu->AddSeparatorItem();
	// Key Map Menu
	menu->AddItem(keyMapMenu = new BMenu("Key Mappings"));
	keyMapMenu->SetRadioMode(true);
	BuildKeyMapMenu(keyMapMenu);

	menu->AddItem(
		new BMenuItem("Remap Keys", new BMessage(MENU_REMAP_KEYS), 'R'));
	menu->AddSeparatorItem();
	// Keyboard
	BuildKeyboardOctavesMenu(submenu = new BMenu("Keyboard Octaves"));
	menu->AddItem(submenu);
	BuildKeyboardRowsMenu(submenu = new BMenu("Keyboard Rows"));
	menu->AddItem(submenu);
	menu->AddSeparatorItem();
	//
	menu->AddItem(new BMenuItem("About ...", new BMessage(B_ABOUT_REQUESTED)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q'));
	menubar->AddItem(menu);
	// Channel
	menu = new BMenu("Channel");
	menu->SetRadioMode(true);
	for (i = 1; i <= 16; i++) {
		if (i == 10)
			sprintf(string, "10 [Drums]");
		else
			sprintf(string, "%d", i);
		menu->AddItem(
			item = new BMenuItem(string, new BMessage(MENU_CHANNEL + i - 1)));
		if (i == 1)
			item->SetMarked(true);
	}
	menubar->AddItem(menu);

	// Group
	groupMenu = menu = new BMenu("Group");
	for (i = 0; i < INSTR_GROUPS; i++) {
		menu->AddItem(item
			= new BMenuItem(instrGroups[i], new BMessage(MENU_GROUPS + i)));
		if (i == 0)
			item->SetMarked(true);
	}
	menu->SetRadioMode(true);
	menubar->AddItem(menu);
	// Instruments
	for (i = 0; i < INSTR_GROUPS; i++) {
		instrumentMenu[i] = menu = new BMenu("Instrument");
		for (j = instrIndizes[i].from; j <= instrIndizes[i].to; j++) {
			menu->AddItem(item = new BMenuItem(instruments[j],
							  new BMessage(MENU_INSTRUMENTS + j)));
			if (j == instrIndizes[i].from)
				item->SetMarked(true);
		}
		menu->SetRadioMode(true);
	}
	menubar->AddItem(instrGroup = instrumentMenu[0]);

	// Octave
	menu = new BMenu("Octave");
	menu->SetRadioMode(true);
	for (i = 1; i <= 11; i++) {
		sprintf(string, "%d", i);
		menu->AddItem(
			item = new BMenuItem(string, new BMessage(MENU_OCTAVE + i - 1)));
		if (i == 4)
			item->SetMarked(true);
	}
	menubar->AddItem(menu);
	/*  // Keyboard
		menu = new BMenu("Keyboard");
		menu->SetRadioMode(true);
		menu->AddItem(item = new BMenuItem("2D", new
		BMessage(MENU_KEYBOARD_2D))); item->SetMarked(true); menu->AddItem(item =
		new BMenuItem("3D", new BMessage(MENU_KEYBOARD_3D)));
		menubar->AddItem(menu);
	*/
	// Port
	midiInPortMenu = new BMenu("Midi In");
	menubar->AddItem(midiInPortMenu);
	midiOutPortMenu = new BMenu("Midi Out");
	menubar->AddItem(midiOutPortMenu);
	// Chords
	chordMenu = menu = new BMenu("Chord");
	menu->SetRadioMode(true);
	menu->AddItem(item = new BMenuItem("Off", new BMessage(MENU_CHORD)));
	item->SetMarked(true);
	LoadChords(menu);
	menubar->AddItem(menu);

	// add view
	view = new View(settings.GetKeyboardOctaves(),
		settings.GetKeyboardRows());

	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.Add(menubar)
		.Add(view)
		.End();

	// Load default instrument definition file
	LoadPatch();
	// Load Key Mapping
	LoadKeyMap(settings.GetKeyMap());

	// make window visible
	Show();

	// Scope Window
	OpenScopeWindow();

	if (Lock()) {
		view->FocusKeyboard();
		Unlock();
	}

	for (i = 0; i < 16; i++)
		view->ProgramChange(i, B_ACOUSTIC_GRAND, system_time());
	//	for (i = 0; i < 15; i++) instrument[i] = B_ACOUSTIC_GRAND;
	//	ProgramChange(1, B_ACOUSTIC_GRAND, true);
}

// Settings
void
AppWindow::LoadSettings()
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
		status_t ret = path.Append(settingsFolder);
		if (ret == B_OK)
			ret = path.Append(settingsFilename);
		if (ret == B_OK) {
			BFile file(path.Path(), B_READ_ONLY);
			if (file.InitCheck() == B_OK) {
				BMessage archive;
				archive.Unflatten(&file);
				GlobalSettings* s =
					(GlobalSettings*) GlobalSettings::Instantiate(&archive);

				if (s != NULL) {
					settings = *s;
					delete s;
				}
			}
		}
	}
}


void
AppWindow::SaveSettings()
{
	BPath path;
	if (settings.HasChanged()
		&& find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
		status_t ret = path.Append(settingsFolder);
		if (ret == B_OK)
			ret = create_directory(path.Path(), 0777);
		if (ret == B_OK)
			ret = path.Append(settingsFilename);
		if (ret == B_OK) {
			BFile file(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
			if (file.InitCheck() == B_OK) {
				BMessage archive;
				settings.Archive(&archive);
				archive.Flatten(&file);
			}
		}
	}
}

// Midi Port
void
AppWindow::PopulatePortMenus()
{
	BMidiRoster* midiManager = BMidiRoster::MidiRoster();
	int i = midiInPortMenu->CountItems();
	while (i >= 0) {
		BMenuItem* item = (midiInPortMenu->RemoveItem(i));
		if (item)
			delete item;
		i--;
	}
	i = midiOutPortMenu->CountItems();
	while (i >= 0) {
		BMenuItem* item = (midiOutPortMenu->RemoveItem(i));
		if (item)
			delete item;
		i--;
	}

	int32 id = 0;
	BMidiConsumer* con = NULL;
	BMidiProducer* pro = NULL;
	while ((con = midiManager->NextConsumer(&id)) != NULL) {
		if (con->IsValid()) {
			BMessage* msg = new BMessage(MENU_OUTPORT);
			msg->AddInt32("port_id", id);
			BMenuItem* item = new BMenuItem(con->Name(), msg);
			midiOutPortMenu->AddItem(item);
			if (view->midiOut->IsConnected(con))
				item->SetMarked(true);
		}
	}
	id = 0;

	while ((pro = midiManager->NextProducer(&id)) != NULL) {
		if (pro->IsValid()) {
			BMessage* msg = new BMessage(MENU_INPORT);
			msg->AddInt32("port_id", id);
			BMenuItem* item = new BMenuItem(pro->Name(), msg);
			midiInPortMenu->AddItem(item);
			if (pro->IsConnected(view))
				item->SetMarked(true);
		}
	}
}

// Synth Defintion File Handling
void
AppWindow::GetSynthEntries()
{
	BPath synthdir;
	// This *will* be == B_SYNTH_DIRECTORY, but may not be on all systems yet:
	find_directory(B_SYSTEM_DATA_DIRECTORY, &synthdir);
	synthdir.Append("synth");
	BDirectory dir(synthdir.Path());
	if (dir.InitCheck() == B_OK) {
		BEntry e;
		while (dir.GetNextEntry(&e) == B_OK)
			synthEntries.AddItem(new BEntry(e));
	}
}


void
AppWindow::SetPatchesMenu(BMenu* menu)
{
	int16 i = 0;
	BEntry* e;
	BMessage* msg = new BMessage(MENU_PATCH_SELECTED);
	// Initial default choice:
	msg->AddInt16("index", -1);
	menu->AddItem(new BMenuItem("<Default>", msg));

	char name[B_FILE_NAME_LENGTH];
	while (NULL != (e = (BEntry*) synthEntries.ItemAt(i))) {
		e->GetName(name);
		msg = new BMessage(MENU_PATCH_SELECTED);
		msg->AddInt16("index", i);
		menu->AddItem(new BMenuItem((const char*) name, msg));
		i++;
	}
}


void
AppWindow::OnPatchSelected(BMessage* msg)
{
	int16 i;
	msg->FindInt16("index", 0, &i);
	if (i < 0) {
		LoadPatch(NULL);
		settings.SetPatch("<Default>");
	} else {
		BEntry* entry = (BEntry*) synthEntries.ItemAt(i);
		LoadPatch(entry);
		char name[B_FILE_NAME_LENGTH];
		entry->GetName(name);
		settings.SetPatch(name);
	}
}


BEntry*
AppWindow::FindPatch(const char* patch)
{
	int16 i = 0;
	BEntry* e = NULL;
	char name[B_FILE_NAME_LENGTH];
	while (NULL != (e = (BEntry*) synthEntries.ItemAt(i))) {
		e->GetName(name);
		if (strcmp(name, patch) == 0)
			return e;
		i++;
	}
	return NULL;
}


bool
AppWindow::LoadPatch(BEntry* entry)
{
	entry_ref e;
	bool ok = false;
	StatusWindow* status = NULL;

	if (entry == NULL) { // Use Synth default
		status = new StatusWindow("Default", Frame());
		ok = midiSynth->LoadSynthData();
	} else if (entry->Exists() && (entry->GetRef(&e) == B_OK)) {
		char name[B_FILE_NAME_LENGTH];
		entry->GetName(name);
		status = new StatusWindow(name, Frame());

		ok = midiSynth->LoadSynthData(&e);
	}

	if (ok) {
		// init be_synth
		be_synth->SetSamplingRate(settings.GetSamplingRate());
		// if (settings.GetReverb() == B_REVERB_NONE)
		//	be_synth->EnableReverb(false);
		// else {
		//	be_synth->EnableReverb(true);
		be_synth->SetReverb(settings.GetReverb());
		//}
		be_synth->SetVoiceLimits(settings.GetMaxSynthVoices(), 0, 0);

		midiSynth->SetVolume(settings.GetMainVolume());
		view->SetMainVolume(settings.GetMainVolume());
		for (int i = 0; i < 16; i++)
			midiSynth->ProgramChange(i, view->GetInstrument(i), system_time());
		EnableMidiSynth(true);
		view->SetChannel(view->GetChannel());
	}

	if (status) {
		status->PostMessage(B_QUIT_REQUESTED);
		return ok;
	}
	return false;
}


void
AppWindow::LoadPatch()
{
	BEntry* entry;
	int i;
	const char* patchname = NULL;
	// try patch name from settings file
	patchname = settings.GetPatch();
	entry = FindPatch(patchname);
	if (entry && LoadPatch(entry)) {
		//		i = synthEntries.IndexOf(entry);
		patchMenu->FindItem(patchname)->SetMarked(true);
		return;
	}
	if (LoadPatch(NULL)) { // check that default works
		patchMenu->FindItem("<Default>")->SetMarked(true);
		return;
	}

	// no patches available, turn off synthesizer support
	midiSynthEnabled = false;
	BMenuItem* item = midiSynthMenu->FindItem(MENU_SYNTH_ENABLED);
	item->SetEnabled(false);
	item->SetMarked(midiSynthEnabled);
	patchMenu->SetEnabled(false);
}

// Load Chords from file
void
AppWindow::LoadChords(BMenu* menu)
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
		path.Append(settingsFolder);
		path.Append("Chords");
		BFile file(path.Path(), B_READ_ONLY);
		if (file.InitCheck() == B_OK) {
			char ch = NextChar(file);
			while (ch != EOF) {
				ch = Skip(ch, file);
				ch = LoadChord(ch, file, menu);
			}
		}
	}
}


void
AppWindow::EnableMidiSynth(bool enable)
{
	if (enable && !midiSynthEnabled) {
		midiSynthEnabled = true;
		view->midiOut->Connect(midiSynth);
	} else if (!enable && midiSynthEnabled) {
		midiSynthEnabled = false;
		view->midiOut->Disconnect(midiSynth);
	}
}


void
AppWindow::PopulateInstrumentMenus()
{
	uchar instr = view->GetInstrument(view->GetChannel());
	int group;
	for (group = 0; group < INSTR_GROUPS; group++) {
		if ((instrIndizes[group].from <= instr)
			&& (instrIndizes[group].to >= instr))
			break;
	}

	BMenuItem* item = groupMenu->FindItem((uint32) group + MENU_GROUPS);
	if (item != NULL)
		item->SetMarked(true);
	ChangeInstrumentMenu(group);

	item = instrGroup->FindItem((uint32) instr + MENU_INSTRUMENTS);
	if (item != NULL)
		item->SetMarked(true);
}


void
AppWindow::ChangeInstrumentMenu(int group)
{
	menubar->RemoveItem(instrGroup);
	instrGroup = instrumentMenu[group];
	menubar->AddItem(instrGroup, 3);
}


void
AppWindow::OnVolumeChanged()
{
	double vol = view->GetMainVolume();
	midiSynth->SetVolume(vol);
	settings.SetMainVolume(vol);
}


void
AppWindow::OnChordSelected()
{
	BMenuItem* item = chordMenu->FindMarked();
	if (item != NULL) {
		const char* text = item->Label();
		if (strcmp(text, "Off") == 0)
			view->ClearChord();
		else
			view->SetChord(((ChordMenuItem*) item)->Chord());
	}
}


bool
AppWindow::OnInstrumentChanged(BMessage* message)
{
	if ((message->what >= MENU_GROUPS)
		&& (message->what < MENU_GROUPS + INSTR_GROUPS)) {
		BMenu* menu = instrumentMenu[message->what - MENU_GROUPS];
		BMenuItem* item = menu->FindMarked();
		if (item != NULL)
			PostMessage(item->Message());
		return true;
	}
	return false;
}


bool
AppWindow::OnProgramChanged(BMessage* message)
{
	if ((message->what >= MENU_INSTRUMENTS)
		&& (message->what < MENU_INSTRUMENTS + INSTRUMENTS)) {
		view->ProgramChange(view->GetChannel(),
			message->what - MENU_INSTRUMENTS, system_time());
		return true;
	}
	return false;
}


bool
AppWindow::OnOctaveChanged(BMessage* message)
{
	if ((message->what >= MENU_OCTAVE) && (message->what < MENU_OCTAVE + 12)) {
		view->SetOctave(message->what - MENU_OCTAVE);
		return true;
	}
	return false;
}


bool
AppWindow::OnChannelChanged(BMessage* message)
{
	if ((message->what >= MENU_CHANNEL)
		&& (message->what < MENU_CHANNEL + 16)) {
		uchar channel = message->what - MENU_CHANNEL;
		view->SetChannel(channel);
		view->AllNotesOff(MIDIIN);
		view->Invalidate();
		return true;
	}
	return false;
}


void
AppWindow::OnSynthesizerEnabled()
{
	EnableMidiSynth(!midiSynthEnabled);
	BMenuItem* item = midiSynthMenu->FindItem(MENU_SYNTH_ENABLED);
	if (item == NULL)
		return;

	if (midiSynthEnabled)
		item->SetLabel("Disable Synthesizer");
	else
		item->SetLabel("Enable Synthesizer");

	patchMenu->SetEnabled(midiSynthEnabled);
}
// Key Mapping Handling
void
AppWindow::BuildKeyMapMenu(BMenu* menu)
{
	BMenuItem* item;
	while (NULL != (item = menu->RemoveItem((int32) 0)))
		delete item;

	BDirectory dir(keyMapDir);
	if (dir.InitCheck() == B_OK) {
		BEntry e;
		char name[B_FILE_NAME_LENGTH];
		while (dir.GetNextEntry(&e) == B_OK) {
			e.GetName(name);
			menu->AddItem(item = new BMenuItem(
				(const char*) name, new BMessage(MENU_KEY_MAP_SELECTED)));
			if (strcmp(settings.GetKeyMap(), name) == 0)
				item->SetMarked(true);
		}
	}
}


void
AppWindow::OnRemapKeys()
{
	BMenuItem* item = midiSynthMenu->FindItem(MENU_REMAP_KEYS);
	if (item != NULL) {
		remapKeys = !remapKeys;
		if (remapKeys) {
			BAlert* help = new BAlert("Help",
				"To remap a key first click with your mouse on a key on the "
				" graphical keyboard then hit the key on the keyboard.\n"
				"You can immediately test the changes. "
				"When you are finished select the menu item Done Remap "
				"Keys in the MidiSynth menu.",
				"Ok");
			help->Go();
			item->SetLabel("Done Remap Keys");
			item->SetShortcut('D', 0);
			view->BeginRemapKeys();
		} else {
			savePanel->Show();
			item->SetLabel("Remap Keys");
			item->SetShortcut('R', 0);
			view->EndRemapKeys();
		}
	}
}


void
AppWindow::OnSave(BMessage* msg)
{
	entry_ref dir;
	BString name;
	msg->FindRef("directory", &dir);
	const char* string;
	msg->FindString("name", &string);
	name = string;
	BDirectory dir2(&dir);
	BFile file(&dir2, name.String(), B_CREATE_FILE | B_WRITE_ONLY);
	if (file.InitCheck() == B_OK) {
		view->GetKeyTable()->Write(file);
		settings.SetKeyMap(name.String());
		BuildKeyMapMenu(keyMapMenu);
	} else {
		BAlert* help = new BAlert("File Error",
			"Error opening key map file for writing. Use "
			"another file name or click on Cancel in the "
			"save file panel.",
			"Ok");
		help->Go();
		savePanel->Show();
	}
}


void
AppWindow::LoadKeyMap(const char* map)
{
	BFile file(&keyMapDir, map, B_READ_ONLY);
	if (file.InitCheck() == B_OK) {
		if (!view->GetKeyTable()->Read(file)) {
			BAlert* help = new BAlert(
				"File Error", "This is no valid key map file.", "Ok");
			help->Go();
			view->GetKeyTable()->DefaultKeyTable();
		}
	} else {
		BAlert* help
			= new BAlert("File Error", "Error opening key map file.", "Ok");
		help->Go();
	}
}


void
AppWindow::OnKeyMapSelected(BMessage* msg)
{
	BMenuItem* item;
	void* ptr;
	if (B_OK == msg->FindPointer("source", 0, &ptr)) {
		item = (BMenuItem*) ptr;
		LoadKeyMap(item->Label());
		settings.SetKeyMap(item->Label());
	}
}

// Reverb
void
AppWindow::BuildReverbMenu(BMenu* menu)
{
	int32 current = settings.GetReverb();
	menu->SetRadioMode(true);
	AddMenuItem(menu, "Off", MENU_REVERB, B_REVERB_NONE, current);
	AddMenuItem(menu, "Closet", MENU_REVERB, B_REVERB_CLOSET, current);
	AddMenuItem(menu, "Garage", MENU_REVERB, B_REVERB_GARAGE, current);
	AddMenuItem(menu, "Ballroom", MENU_REVERB, B_REVERB_BALLROOM, current);
	AddMenuItem(menu, "Cavern", MENU_REVERB, B_REVERB_CAVERN, current);
	AddMenuItem(menu, "Dungeon", MENU_REVERB, B_REVERB_DUNGEON, current);
}


void
AppWindow::OnReverb(BMessage* msg)
{
	int32 data;
	if (B_OK == msg->FindInt32("data", &data)) {
		reverb_mode reverb = (reverb_mode) data;
		// this does not work: a once disabled reverb can not be
		// enabled without reloading the samples!
		// (probably not true in Haiku, but...)
		// if (reverb == B_REVERB_NONE)
		//	be_synth->EnableReverb(false);
		// else {
		//	be_synth->EnableReverb(true);
		be_synth->SetReverb(reverb);
		//}
		settings.SetReverb(reverb);
	}
}

// SamplingRate
void
AppWindow::BuildSamplingRateMenu(BMenu* menu)
{
	int32 current = settings.GetSamplingRate();
	menu->SetRadioMode(true);
	AddMenuItem(menu, "22050", MENU_SAMPLING_RATE, 22050, current);
	AddMenuItem(menu, "44100", MENU_SAMPLING_RATE, 44100, current);
	AddMenuItem(menu, "48000", MENU_SAMPLING_RATE, 48000, current);
}


void
AppWindow::OnSamplingRate(BMessage* msg)
{
	int32 data;
	if (B_OK == msg->FindInt32("data", &data)) {
		be_synth->SetSamplingRate(data);
		settings.SetSamplingRate(data);
	}
}


// Max Synth Voices
void
AppWindow::BuildMaxSynthVoicesMenu(BMenu* menu)
{
	BMenuItem* item;
	char string[4];
	menu->SetRadioMode(true);
	for (int i = 1; i <= 32; i++) {
		sprintf(string, "%d", i);
		menu->AddItem(
			item = new BMenuItem(string, NewMessage(MENU_MAX_SYNTH_VOICES, i)));
		if (i == settings.GetMaxSynthVoices())
			item->SetMarked(true);
	}
}


void
AppWindow::OnMaxSynthVoices(BMessage* msg)
{
	int32 data;
	if (B_OK == msg->FindInt32("data", &data)) {
		be_synth->SetVoiceLimits(data, 0, be_synth->LimiterThreshhold());
		settings.SetMaxSynthVoices(data);
	}
}


// Keyboard
void
AppWindow::BuildKeyboardOctavesMenu(BMenu* menu)
{
	BMenuItem* item;
	char string[4];
	menu->SetRadioMode(true);
	for (int i = 1; i <= 11; i++) {
		sprintf(string, "%d", i);
		menu->AddItem(
			item = new BMenuItem(string, NewMessage(MENU_KEYBOARD_OCTAVES, i)));
		if (i == settings.GetKeyboardOctaves())
			item->SetMarked(true);
	}
}


void
AppWindow::OnKeyboardOctaves(BMessage* msg)
{
	int32 data;
	if (B_OK == msg->FindInt32("data", &data)) {
		Keyboard2D* k = (Keyboard2D*) view->GetKeyboard();
		k->SetOctaves(data);
		k->LayoutChanged();
		if (Lock()) {
			k->Invalidate();
			Unlock();
		}
		settings.SetKeyboardOctaves(data);
	}
}


void
AppWindow::BuildKeyboardRowsMenu(BMenu* menu)
{
	BMenuItem* item;
	char string[4];
	menu->SetRadioMode(true);
	for (int i = 1; i <= 2; i++) {
		sprintf(string, "%d", i);
		menu->AddItem(
			item = new BMenuItem(string, NewMessage(MENU_KEYBOARD_ROWS, i)));
		if (i == settings.GetKeyboardRows())
			item->SetMarked(true);
	}
}


void
AppWindow::OnKeyboardRows(BMessage* msg)
{
	int32 data;
	if (B_OK == msg->FindInt32("data", &data)) {
		Keyboard2D* k = (Keyboard2D*) view->GetKeyboard();
		k->SetRows(data);
		k->LayoutChanged();
		if (Lock()) {
			k->Invalidate();
			Unlock();
		}
		settings.SetKeyboardRows(data);
	}
}


void
AppWindow::OnReset()
{
	view->AllNotesOff(true, system_time());
	view->ReleaseNotes(MOUSE | KEYBOARD | MIDIIN | CHORD);
}

// Scope Window
void
AppWindow::OpenScopeWindow()
{
	BRect rect;
	float w, h;
	settings.GetScopeWindowSize(w, h);
	rect.Set(0, 0, w, h);
	rect.OffsetTo(settings.GetScopeWindowPosition());
	scopeWindow = new ScopeWindow(this, &settings, rect);
	scopeMenu->SetMarked(true);
	Activate();
}


void
AppWindow::OnScope()
{
	if (scopeMenu->IsMarked() && scopeWindow) {
		scopeWindow->PostMessage(B_QUIT_REQUESTED);
		scopeWindow = NULL;
	} else if (!scopeMenu->IsMarked() && !scopeWindow)
		OpenScopeWindow();
}


void
AppWindow::OnScopeWindowClosed()
{
	scopeWindow = NULL; // is not NULL when a user closed window
	scopeMenu->SetMarked(false);
}

// Standard Window Events
void
AppWindow::MenusBeginning()
{
	PopulateInstrumentMenus();
	PopulatePortMenus();
}


void
AppWindow::FrameMoved(BPoint p)
{
	// move scope window
	BPoint pos = p - settings.GetWindowPosition();
	if (scopeWindow && scopeWindow->Lock()) {
		scopeWindow->MoveBy(pos.x, pos.y);
		scopeWindow->Unlock();
	} else // even it is not open
		settings.SetScopeWindowPosition(
			pos + settings.GetScopeWindowPosition());
	settings.SetWindowPosition(p);
}


void
AppWindow::FrameResized(float w, float h)
{
	settings.SetWindowSize(w, h);
}


void
AppWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {

		case B_ABOUT_REQUESTED:
			AboutRequested();
			break;

		case MENU_RESET:
			OnReset();
			break;

		case MENU_KEYBOARD_2D:
			view->SetKeyboard(0);
			break;

		case MENU_KEYBOARD_3D:
			view->SetKeyboard(1);
			break;

		case MENU_INPORT:
		{
			BMidiRoster* midiManager = BMidiRoster::MidiRoster();
			int32 id = message->FindInt32("port_id");
			BMidiProducer* pro = midiManager->FindProducer(id);
			if (pro) {
				if (pro->IsConnected(view))
					pro->Disconnect(view);
				else
					pro->Connect(view);
			}
		} break;
		case MENU_OUTPORT:
		{
			BMidiRoster* midiManager = BMidiRoster::MidiRoster();
			int32 id = message->FindInt32("port_id");
			BMidiConsumer* con = midiManager->FindConsumer(id);
			if (con) {
				if (view->midiOut->IsConnected(con))
					view->midiOut->Disconnect(con);
				else
					view->midiOut->Connect(con);
			}
		} break;
		case MENU_SYNTH_ENABLED:
			OnSynthesizerEnabled();
			break;

		case MSG_PITCH_BEND_CHANGED:
			view->PitchBendChanged();
			break;

		case MSG_CH_PRESSURE_CHANGED:
			view->ChannelPressureChanged();
			break;

		case MSG_PAN_CHANGED:
			view->PanChanged();
			break;

		case MSG_VOLUME_CHANGED:
			OnVolumeChanged();
			break;

		case MSG_VELOCITY_CHANGED:
			view->VelocityChanged();
			break;

		case MENU_PATCH_SELECTED:
			OnPatchSelected(message);
			break;

		case MENU_CHORD:
			OnChordSelected();
			break;

		case MENU_REMAP_KEYS:
			OnRemapKeys();
			break;

		case B_SAVE_REQUESTED:
			OnSave(message);
			break;

		case MENU_KEY_MAP_SELECTED:
			OnKeyMapSelected(message);
			break;

		case MENU_REVERB:
			OnReverb(message);
			break;

		case MENU_SAMPLING_RATE:
			OnSamplingRate(message);
			break;

		case MENU_MAX_SYNTH_VOICES:
			OnMaxSynthVoices(message);
			break;

		case MENU_KEYBOARD_OCTAVES:
			OnKeyboardOctaves(message);
			break;

		case MENU_KEYBOARD_ROWS:
			OnKeyboardRows(message);
			break;

		case MSG_SCOPE_CLOSED_NOTIFY:
			OnScopeWindowClosed();
			break;

		case MENU_SCOPE:
			OnScope();
			break;

		default:
			if (!OnInstrumentChanged(message) && !OnProgramChanged(message) &&
			!OnOctaveChanged(message) && !OnChannelChanged(message) /*&&
			!OnPortChanged(message)*/)
				BWindow::MessageReceived(message);
	}
}


bool
AppWindow::QuitRequested()
{
	SaveSettings();

	midiSynth->Release();
	view->midiOut->Release();

	view->LockLooper();
	view->RemoveSelf();
	view->Release();

	menubar->RemoveItem(instrGroup);
	for (int i = 0; i < INSTR_GROUPS; i++)
		delete instrumentMenu[i];

	BEntry* entry;
	for (int32 i = 0; (entry = (BEntry*) synthEntries.ItemAt(i)) != NULL; i++)
		delete entry;
	synthEntries.MakeEmpty();

	delete savePanel;

	be_app->PostMessage(B_QUIT_REQUESTED);
	return (true);
}


void
AppWindow::AboutRequested()
{
	BAlert* about = new BAlert(APPLICATION,
		APPLICATION " " VERSION "\n"
					"is free software under the GPL.\n\n"
					"Written 1999, 2000.\n\n"
					"By Michael Pfeiffer.\n\n"
					"Midi Kit contributions from Dan Walton.\n\n"
					"Haiku adjustments (2013) by Pete Goodeve.\n",
		"Close");
	about->Go();
}


App::App()
	:
	BApplication("application/x-vnd.midisynth")
{
	BRect aRect;
	// set up a rectangle and instantiate a new window
	aRect.Set(100, 80, 410, 380);
	window = NULL;
	window = new AppWindow(aRect);
}


int
main(int argc, char* argv[])
{
	be_app = NULL;
	new App();
	be_app->Run();
	delete be_app;
	//	delete be_synth;	-- not needed, I believe
	return 0;
}
