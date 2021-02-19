/*
 * Copyright 2000-2021. All rights reserved.
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
#include "MsgConsts.h"
#include "Scope.h"
#include "StatusWindow.h"

#include <AboutWindow.h>
#include <Catalog.h>
#include <LayoutBuilder.h>
#include <MidiRoster.h>
#include <PathFinder.h>
#include <Roster.h>
#include <StorageKit.h>
#include <StringList.h>
#include <SupportKit.h>

#include <ctype.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Instruments"

#define INSTR_GROUPS 16
#define INSTRUMENTS 128

static const char settingsFolder[] = "MidiSynth";
static const char settingsFilename[] = "MidiSynth";

static char const* instruments[128] = {
	// Pianos (0-7)
	B_TRANSLATE("Acoustic grand"), B_TRANSLATE("Bright grand"),
	B_TRANSLATE("Electric grand"), B_TRANSLATE("Honky tonk"),
	B_TRANSLATE("Electric piano"), B_TRANSLATE("Electric piano 2"),
	B_TRANSLATE("Harpsichord"), B_TRANSLATE("Clavichord"),
	// Tuned Idiophones (8-15)
	B_TRANSLATE("Celesta"), B_TRANSLATE("Glockenspiel"),
	B_TRANSLATE("Music box"), B_TRANSLATE("Vibraphone"), B_TRANSLATE("Marimba"),
	B_TRANSLATE("Xylophone"), B_TRANSLATE("Tubular bells"),
	B_TRANSLATE("Dulcimer"),
	// Organs (16-23)
	B_TRANSLATE("Drawbar organ"), B_TRANSLATE("Percussive organ"),
	B_TRANSLATE("Rock organ"), B_TRANSLATE("Church Organ"),
	B_TRANSLATE("Reed organ"), B_TRANSLATE("Accordion"),
	B_TRANSLATE("Harmonica"), B_TRANSLATE("Tango Accordion"),
	// Guitars (24-31)
	B_TRANSLATE("Acoustic guitar nylon"), B_TRANSLATE("Acoustic guitar steel"),
	B_TRANSLATE("Electric guitar jazz"), B_TRANSLATE("Electric guitar clean"),
	B_TRANSLATE("Electric guitar muted"), B_TRANSLATE("Overdriven guitar"),
	B_TRANSLATE("Distortion guitar"), B_TRANSLATE("Guitar harmonics"),
	// Basses (32-39)
	B_TRANSLATE("Acoustic bass"), B_TRANSLATE("Electric bass finger"),
	B_TRANSLATE("Electric bass pick"), B_TRANSLATE("Fretless bass"),
	B_TRANSLATE("Slap bass 1"), B_TRANSLATE("Slap bass 2"),
	B_TRANSLATE("Synth bass 1"), B_TRANSLATE("Synth bass 2"),
	// Strings And Timpani (40-47)
	B_TRANSLATE("Violin"), B_TRANSLATE("Viola"), B_TRANSLATE("Cello"),
	B_TRANSLATE("Contrabass"), B_TRANSLATE("Tremolo strings"),
	B_TRANSLATE("Pizzicato strings"), B_TRANSLATE("Orchestral strings"),
	B_TRANSLATE("Timpani"),
	// Ensemble Strings And Voices (48-55)
	B_TRANSLATE("String ensemble 1"), B_TRANSLATE("String ensemble 2"),
	B_TRANSLATE("Synth strings 1"), B_TRANSLATE("Synth strings 2"),
	B_TRANSLATE("Voice aah"), B_TRANSLATE("Voice ooh"),
	B_TRANSLATE("Synth voice"), B_TRANSLATE("Orchestra hit"),
	// Brass (56-63)
	B_TRANSLATE("Trumpet"), B_TRANSLATE("Trombone"), B_TRANSLATE("Tuba"),
	B_TRANSLATE("Muted trumpet"), B_TRANSLATE("French horn"),
	B_TRANSLATE("Brass section"), B_TRANSLATE("Synth brass 1"),
	B_TRANSLATE("Synth brass 2"),
	// Reeds (64-71)
	B_TRANSLATE("Soprano sax"), B_TRANSLATE("Alto sax"),
	B_TRANSLATE("Tenor sax"), B_TRANSLATE("Baritone sax"), B_TRANSLATE("Oboe"),
	B_TRANSLATE("English horn"), B_TRANSLATE("Bassoon"), B_TRANSLATE("Clarinet"),
	// Pipes (72-79)
	B_TRANSLATE("Piccolo"), B_TRANSLATE("Flute"), B_TRANSLATE("Recorder"),
	B_TRANSLATE("Pan flute"), B_TRANSLATE("Blown bottle"),
	B_TRANSLATE("Shakuhachi"), B_TRANSLATE("Whistle"), B_TRANSLATE("Ocarina"),
	// Synth Leads (80-87)
	B_TRANSLATE("Square wave"), B_TRANSLATE("Sawtooth wave"),
	B_TRANSLATE("Calliope"), B_TRANSLATE("Chiff"), B_TRANSLATE("Charang"),
	B_TRANSLATE("Voice"), B_TRANSLATE("Fifths"), B_TRANSLATE("Bass lead"),
	// Synth Pads (88-95)
	B_TRANSLATE("New age"), B_TRANSLATE("Warm"), B_TRANSLATE("Polysynth"),
	B_TRANSLATE("Choir"), B_TRANSLATE("Bowed"), B_TRANSLATE("Metallic"),
	B_TRANSLATE("Halo"), B_TRANSLATE("Sweep"),
	// Musical Effects (96-103)
	B_TRANSLATE("Fx1"), B_TRANSLATE("Fx2"), B_TRANSLATE("Fx3"),
	B_TRANSLATE("Fx4"), B_TRANSLATE("Fx5"), B_TRANSLATE("Fx6"),
	B_TRANSLATE("Fx7"), B_TRANSLATE("Fx8"),
	// Ethnic (104-111)
	B_TRANSLATE("Sitar"), B_TRANSLATE("Banjo"), B_TRANSLATE("Shamisen"),
	B_TRANSLATE("Koto"), B_TRANSLATE("Kalimba"), B_TRANSLATE("Bagpipe"),
	B_TRANSLATE("Fiddle"), B_TRANSLATE("Shanai"),
	// Percussion (112-119)
	B_TRANSLATE("Tinkle bell"), B_TRANSLATE("Agogo"),
	B_TRANSLATE("Steel drums"), B_TRANSLATE("Woodblock"),
	B_TRANSLATE("Taiko drums"), B_TRANSLATE("Melodic tom"),
	B_TRANSLATE("Synth drum"), B_TRANSLATE("Reverse cymbal"),
	// Sound Effects (120-127)
	B_TRANSLATE("Fret noise"), B_TRANSLATE("Breath noise"),
	B_TRANSLATE("Seashore"), B_TRANSLATE("Bird tweet"),
	B_TRANSLATE("Telephone"), B_TRANSLATE("Helicopter"),
	B_TRANSLATE("Applause"), B_TRANSLATE("Gunshot")};

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Groups"

static char const* instrGroups[INSTR_GROUPS] = {B_TRANSLATE("Pianos"),
	B_TRANSLATE("Tuned idiophones"), B_TRANSLATE("Organs"),
	B_TRANSLATE("Guitars"), B_TRANSLATE("Basses"),
	B_TRANSLATE("Strings and timpani"),
	B_TRANSLATE("Ensemble strings and voices"), B_TRANSLATE("Brass"),
	B_TRANSLATE("Reeds"), B_TRANSLATE("Pipes"), B_TRANSLATE("Synth leads"),
	B_TRANSLATE("Synth pads"), B_TRANSLATE("Musical effects"),
	B_TRANSLATE("Ethnic"), B_TRANSLATE("Percussion"),
	B_TRANSLATE("Sound effects")};

static struct {
	uchar from, to;
}
instrIndizes[INSTR_GROUPS] = {{0, 7}, {8, 15}, {16, 23}, {24, 31}, {32, 39},
	{40, 47}, {48, 55}, {56, 63}, {64, 71}, {72, 79}, {80, 87}, {88, 95},
	{96, 103}, {104, 111}, {112, 119}, {120, 127}};

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Application"

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
	BWindow(aRect, B_TRANSLATE_SYSTEM_NAME("MidiSynth"), B_TITLED_WINDOW,
		B_ASYNCHRONOUS_CONTROLS | B_NOT_ZOOMABLE)
{
	SetSizeLimits(460, 10000, 155, 10000);
	midiSynth = new CInternalSynth(B_TRANSLATE_SYSTEM_NAME("MidiSynth"));
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

	midiSynthMenu = menu = new BMenu(B_TRANSLATE_SYSTEM_NAME("MidiSynth"));

	// Reset
	menu->AddItem(new BMenuItem(B_TRANSLATE("Reset"), new BMessage(MENU_RESET),
		'R'));
	menu->AddItem(scopeMenu = new BMenuItem(B_TRANSLATE("Scope"),
		new BMessage(MENU_SCOPE), 'V'));
	menu->AddSeparatorItem();

	// Synthesizer
	menu->AddItem(item = new BMenuItem(B_TRANSLATE("Disable synthesizer"),
		new BMessage(MENU_SYNTH_ENABLED), 'S'));
	patchMenu = new BMenu(B_TRANSLATE("SoundFont"));
	patchMenu->SetRadioMode(true);
	SetPatchesMenu(patchMenu);
	menu->AddItem(patchMenu);

	// Reverb
	BuildReverbMenu(submenu = new BMenu(B_TRANSLATE("Reverb")));
	menu->AddItem(submenu);
	// Sampling Rate
	BuildSamplingRateMenu(submenu = new BMenu(B_TRANSLATE("Sampling rate")));
	menu->AddItem(submenu);

	// Max Synth Voices
	BuildMaxSynthVoicesMenu(submenu = new BMenu(B_TRANSLATE("Max synth voices")));
	menu->AddItem(submenu);
	menu->AddSeparatorItem();

	// Key Map Menu
	menu->AddItem(keyMapMenu = new BMenu(B_TRANSLATE("Key mappings")));
	keyMapMenu->SetRadioMode(true);
	BuildKeyMapMenu(keyMapMenu);

	menu->AddItem(new BMenuItem(B_TRANSLATE("Remap keys"),
		new BMessage(MENU_REMAP_KEYS),'R'));
	menu->AddSeparatorItem();

	// Keyboard
	BuildKeyboardOctavesMenu(submenu = new BMenu(B_TRANSLATE("Keyboard octaves")));
	menu->AddItem(submenu);
	BuildKeyboardRowsMenu(submenu = new BMenu(B_TRANSLATE("Keyboard rows")));
	menu->AddItem(submenu);
	menu->AddSeparatorItem();

	menu->AddItem(new BMenuItem(B_TRANSLATE("About" B_UTF8_ELLIPSIS),
		new BMessage(B_ABOUT_REQUESTED)));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem(B_TRANSLATE("Quit"),
		new BMessage(B_QUIT_REQUESTED), 'Q'));
	menubar->AddItem(menu);

	// Channel
	menu = new BMenu(B_TRANSLATE("Channel"));
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
	menu->SetLabelFromMarked(true);

	// Instruments
	for (i = 0; i < INSTR_GROUPS; i++) {
		instrumentMenu[i] = menu = new BMenu("Instrument");
		for (j = instrIndizes[i].from; j <= instrIndizes[i].to; j++) {
			menu->AddItem(item = new BMenuItem(instruments[j],
							  new BMessage(MENU_INSTRUMENTS + j)));
			if (j == instrIndizes[i].from)
				item->SetMarked(true);
		}
		menu->SetLabelFromMarked(true);
	}

	// Octave
	menu = new BMenu(B_TRANSLATE("Octave"));
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
	midiInPortMenu = new BMenu(B_TRANSLATE("Midi in"));
	menubar->AddItem(midiInPortMenu);
	midiOutPortMenu = new BMenu(B_TRANSLATE("Midi out"));
	menubar->AddItem(midiOutPortMenu);

	// Chords
	chordMenu = menu = new BMenu("Chord");
	menu->SetLabelFromMarked(true);
	menu->AddItem(item = new BMenuItem(B_TRANSLATE_COMMENT("Off", "Chord off"),
		new BMessage(MENU_CHORD)));
	item->SetMarked(true);
	LoadChords(menu);

	// add popup menu view
	BMenuField* chordField = new BMenuField(B_TRANSLATE("Chord:"), chordMenu);
	BMenuField* groupField = new BMenuField(NULL, groupMenu);
	instrumentField = new BMenuField(NULL, instrGroup = instrumentMenu[0]);

	BFont font(be_plain_font);
	float stringwidth = font.StringWidth("Ensemble Strings and Voices") + 32;

	groupField->SetExplicitMinSize(BSize(stringwidth, 0));

	popView = BLayoutBuilder::Group<>()
		.AddGroup(B_VERTICAL, B_USE_SMALL_SPACING)
			.Add(chordField)
			.Add(groupField)
			.Add(instrumentField)
			.End()
		.View();

	// add view
	view = new View(settings.GetKeyboardOctaves(), settings.GetKeyboardRows(),
		popView);

	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.Add(menubar)
		.Add(view)
		.End();

	// Load default instrument definition file
	LoadPatch();
	LoadKeyMap(settings.GetKeyMap());

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
	BPath path;
	BStringList paths;
	BPathFinder pathFinder;
	BEntry entry;
	status_t error = pathFinder.FindPaths(B_FIND_PATH_DATA_DIRECTORY,
		"synth", paths);

	for (int i = 0; i < paths.CountStrings(); ++i) {
		if (error == B_OK && path.SetTo(paths.StringAt(i)) == B_OK) {
			BDirectory dir(path.Path());
			if (dir.InitCheck() == B_OK) {
				BEntry entry;
				while (dir.GetNextEntry(&entry) == B_OK)
					synthEntries.AddItem(new BEntry(entry));
			}
		}
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
	menu->AddItem(new BMenuItem(B_TRANSLATE_COMMENT("<Default>",
		"Default SoundFont"), msg));

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
		status = new StatusWindow(B_TRANSLATE_COMMENT("Default",
			"Default SoundFont"), Frame());
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
		patchMenu->FindItem(B_TRANSLATE_COMMENT("<Default>",
			"Default SoundFont"))->SetMarked(true);
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
	instrumentField->MenuBar()->RemoveItem(instrGroup);
	instrGroup = instrumentMenu[group];
	instrumentField->MenuBar()->AddItem(instrGroup);
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
		if (strcmp(text, B_TRANSLATE_COMMENT("Off", "Chord off")) == 0)
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
		if (item != NULL) {
			// Group changed, force update Instrument menu
			ChangeInstrumentMenu(message->what - MENU_GROUPS);
			PostMessage(item->Message());
		}
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
		item->SetLabel(B_TRANSLATE("Disable synthesizer"));
	else
		item->SetLabel(B_TRANSLATE("Enable synthesizer"));

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
			BAlert* help = new BAlert(B_TRANSLATE("Help"), B_TRANSLATE(
				"To remap a key, first click with your mouse on a key on the "
				"graphical keyboard, then hit the key on the keyboard.\n"
				"You can immediately test the changes. "
				"When you are finished, select the menu item 'Done remapping' "
				"from the 'MidiSynth' menu."),
				B_TRANSLATE("OK"));
			help->Go();
			item->SetLabel("Done remapping");
			item->SetShortcut('D', 0);
			view->BeginRemapKeys();
		} else {
			savePanel->Show();
			item->SetLabel("Remap keys");
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
		BAlert* help = new BAlert(B_TRANSLATE("File error"), B_TRANSLATE(
			"Couldn't save key map file. Please try another file name "
			"or click on 'Cancel' in the save panel."),
			B_TRANSLATE("OK"));
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
			BAlert* help = new BAlert(B_TRANSLATE("File error"), B_TRANSLATE(
				"This is no valid key map file."),
				B_TRANSLATE("OK"));
			help->Go();
			view->GetKeyTable()->DefaultKeyTable();
		}
	} else {
		BAlert* help
			= new BAlert(B_TRANSLATE("File error"), B_TRANSLATE(
				"Error opening key map file."),
				B_TRANSLATE("OK"));
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
	AddMenuItem(menu, B_TRANSLATE("Off"), MENU_REVERB, B_REVERB_NONE,
		current);
	AddMenuItem(menu, B_TRANSLATE("Closet"), MENU_REVERB, B_REVERB_CLOSET,
		current);
	AddMenuItem(menu, B_TRANSLATE("Garage"), MENU_REVERB, B_REVERB_GARAGE,
		current);
	AddMenuItem(menu, B_TRANSLATE("Ballroom"), MENU_REVERB, B_REVERB_BALLROOM,
		current);
	AddMenuItem(menu, B_TRANSLATE("Cavern"), MENU_REVERB, B_REVERB_CAVERN,
		current);
	AddMenuItem(menu, B_TRANSLATE("Dungeon"), MENU_REVERB, B_REVERB_DUNGEON,
		current);
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

	instrumentField->MenuBar()->RemoveItem(instrGroup);
	for (int i = 0; i < INSTR_GROUPS; i++)
		delete instrumentMenu[i];

	midiSynth->Release();
	view->midiOut->Release();

	view->LockLooper();
	view->RemoveSelf();
	view->Release();

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
	BAboutWindow* about = new BAboutWindow(
		B_TRANSLATE_SYSTEM_NAME("MidiSynth"), SIGNATURE);
	const char* extraCopyrights[] = {
		"2013 Pete Goodeve",
		"2020 Humdinger",
		NULL
	};
	const char* authors[] = {
		B_TRANSLATE("Michael Pfeiffer (original author)"),
		"Dan Walton",
		"Pete Goodeve",
		"Humdinger",
		NULL
	};
	about->AddCopyright(1999, "Michael Pfeiffer", extraCopyrights);
	about->AddAuthors(authors);
	about->Show();
}


App::App()
	:
	BApplication(SIGNATURE)
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
