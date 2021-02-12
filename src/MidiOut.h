/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#ifndef MIDI_OUT_H
#define MIDI_OUT_H

#include <MidiProducer.h>

class MidiOut : public BMidiLocalProducer {
public:
  MidiOut(const char *name = "MidiSynth");
  /*
          void ChannelPressure(uchar channel, uchar pressure, bigtime_t time =
     system_time()); void PitchBend(uchar channel, uchar lsb, uchar msb,
     bigtime_t time = system_time()); void NoteOn(uchar channel, uchar note,
     uchar velocity, bigtime_t time = system_time()); void NoteOff(uchar
     channel, uchar note, uchar velocity, bigtime_t time = system_time()); void
     ProgramChange(uchar channel, uchar progNum, bigtime_t time =
     system_time());
  */
  void AllNotesOff(bool justChannel, bigtime_t time);
  // void SetVolume(double scale);
};

#endif
