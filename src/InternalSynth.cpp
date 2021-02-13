/* ===================================================================== *
 * InternalSynth.cpp (MeV/Midi)
 * ===================================================================== */

#include "InternalSynth.h"
#include "InternalSynthIconBits.h"

// Application Kit
#include <Message.h>
// Interface Kit
#include <Bitmap.h>
// Midi Kit
#include <Midi.h>
#include <MidiSynth.h>

// ---------------------------------------------------------------------------
// Constructor/Destructor

CInternalSynth::CInternalSynth(char* name)
	:
	BMidiLocalConsumer(name),
	m_midiSynth(NULL)
{
	BMessage props;
	GetProperties(&props);
	props.AddBool("mev:internalSynth", true);

	BBitmap* largeIcon = GetIcon(B_LARGE_ICON);
	if (largeIcon) {
		props.AddData("be:large_icon", 'ICON', largeIcon->Bits(),
			largeIcon->BitsLength());
		delete largeIcon;
	}
	BBitmap* smallIcon = GetIcon(B_MINI_ICON);
	if (smallIcon) {
		props.AddData(
			"be:mini_icon", 'MICN', smallIcon->Bits(), smallIcon->BitsLength());
		delete smallIcon;
	}
	SetProperties(&props);
}


CInternalSynth::~CInternalSynth()
{
	delete m_midiSynth;
}

// ---------------------------------------------------------------------------
// Accessors

BBitmap*
CInternalSynth::GetIcon(icon_size size) const
{
	BRect rect(0.0, 0.0, size - 1.0, size - 1.0);
	BBitmap* bitmap = new BBitmap(rect, B_CMAP8);

	switch (size) {
		case B_MINI_ICON:
		{
			bitmap->SetBits(MINI_SYNTH_ICON_BITS, size * size, 0, B_CMAP8);
			break;
		}
		case B_LARGE_ICON:
		{
			bitmap->SetBits(LARGE_SYNTH_ICON_BITS, size * size, 0, B_CMAP8);
			break;
		}
	}

	return bitmap;
}

// ---------------------------------------------------------------------------
// Operations

void
CInternalSynth::Init()
{
	if (m_midiSynth == NULL)
		m_midiSynth = new BMidiSynth;
}


bool
CInternalSynth::LoadSynthData(entry_ref* ref)
{
	if (B_OK == be_synth->LoadSynthData(ref)) {
		m_midiSynth->EnableInput(true, true);
		return true;
	}
	return false;
}


bool
CInternalSynth::LoadSynthData(void)
{
	if (B_OK == be_synth->LoadSynthData(B_BIG_SYNTH)) {
		m_midiSynth->EnableInput(true, true);
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
// BMidiLocalConsumer Implementation

void
CInternalSynth::NoteOff(
	uchar channel, uchar note, uchar velocity, bigtime_t time)
{
	m_midiSynth->NoteOff(channel + 1, note, velocity, time / 1000);
}


void
CInternalSynth::NoteOn(
	uchar channel, uchar note, uchar velocity, bigtime_t time)
{
	m_midiSynth->NoteOn(channel + 1, note, velocity, time / 1000);
}


void
CInternalSynth::KeyPressure(
	uchar channel, uchar note, uchar pressure, bigtime_t time)
{
	m_midiSynth->KeyPressure(channel + 1, note, pressure, time / 1000);
}


void
CInternalSynth::ControlChange(
	uchar channel, uchar controlNumber, uchar controlValue, bigtime_t time)
{
	m_midiSynth->ControlChange(
		channel + 1, controlNumber, controlValue, time / 1000);
}


void
CInternalSynth::ProgramChange(
	uchar channel, uchar programNumber, bigtime_t time)
{
	m_midiSynth->ProgramChange(channel + 1, programNumber, time / 1000);
}


void
CInternalSynth::ChannelPressure(uchar channel, uchar pressure, bigtime_t time)
{
	m_midiSynth->ChannelPressure(channel + 1, pressure, time / 1000);
}


void
CInternalSynth::PitchBend(uchar channel, uchar lsb, uchar msb, bigtime_t time)
{
	m_midiSynth->PitchBend(channel + 1, lsb, msb, time / 1000);
}

// END - InternalSynth.cpp
