/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#ifndef VIEW_H
#define VIEW_H
#include "KeyTable.h"
#include "Keyboard.h"
#include "MidiOut.h"
#include <InterfaceKit.h>
#include <MidiConsumer.h>
// input devices:
#define MOUSE 1
#define KEYBOARD 2
#define MIDIIN 4
#define CHORD 8

class View : public BView, public BMidiLocalConsumer /* for Midi input */ {
  KeyTable keyTable;
  struct {
    uchar device; // input device 1..mouse, 2..keyboard, 4..midi in, 8..chord
    uchar count;  //
  } notePlaying[128];

  int noteOffset;
  int channel; // , chPressure, velocity, pitch, pan;
  BSlider *chPressureSlider, *pitchBendSlider, *volumeSlider, *velocitySlider,
      *panSlider;

  struct {
    int pressure, velocity, pitch, pan, instrument;
  } channels[16];

  Keyboard *keyboard;  // view of the current keyboard
  int currentKeyboard; // 0 .. 2D, 1 .. 3D

  float subViewTop; // top of keyboard view

  struct {
    int len;
    int notes[256];
    bool enabled;
  } chord;

  struct {
    bool remap;
    uchar note;
  } keys;

public:
  MidiOut *midiOut;

  View(BRect rect, int16 octaves, int16 rows);
  ~View();
  uchar Key2Note(uchar key);
  uchar NotePlaying(uchar note);
  inline int GetNoteOffset() { return noteOffset; }
#if 0 
	void KeyDown(const char *bytes, int32 numBytes);
	void KeyUp(const char *bytes, int32 numBytes);
#endif
  void KeyDown(int32 key, int32 modifiers);
  void KeyUp(int32 key, int32 modifiers);

  void FocusKeyboard();

  void ChannelPressureChanged();
  void VelocityChanged();
  void PitchBendChanged();
  void PanChanged();

  void SetOctave(uchar octave);
  void SetChannel(int channel);
  uchar GetChannel(void) { return channel; }
  uchar GetChannelPressure(uchar channel) { return channels[channel].pressure; }
  uchar GetVelocity(uchar channel) { return channels[channel].velocity; }
  uchar GetPitchBend(uchar channel) { return channels[channel].pitch; }
  uchar GetPan(uchar channel) { return channels[channel].pan; }
  uchar GetInstrument(uchar channel) { return channels[channel].instrument; }
  void SetChannelPressure(uchar channel, uchar pressure) {
    channels[channel].pressure = pressure;
  }
  void SetVelocity(uchar channel, uchar velocity) {
    channels[channel].velocity = velocity;
  }
  void SetPitchBend(uchar channel, uchar pitch) {
    channels[channel].pitch = pitch;
  }
  void SetPan(uchar channel, uchar pan) { channels[channel].pan = pan; }
  void SetInstrument(uchar channel, uchar instrument) {
    channels[channel].instrument = instrument;
  }

  void SetKeyboard(int index);
  Keyboard *GetKeyboard();

  void Invalidate(void);
  bool LockLooper(void) { // ambiguous in Haiku
    BView::LockLooper();
  }

  void NoteOn(uchar note, uchar source, bool map);
  void NoteOff(uchar note, uchar source, bool map);
  void ReleaseNotes(uchar source);
  void AllNotesOff(uchar source);

  void SetChord(const char *chord);
  void ClearChord();

  void SetMainVolume(double vol);
  double GetMainVolume();

  KeyTable *GetKeyTable();
  void BeginRemapKeys();
  void EndRemapKeys();

  // Midi Input
  virtual void ChannelPressure(uchar channel, uchar pressure, bigtime_t time);
  virtual void PitchBend(uchar channel, uchar lsb, uchar msb, bigtime_t time);
  virtual void NoteOn(uchar channel, uchar note, uchar velocity,
                      bigtime_t time);
  virtual void NoteOff(uchar channel, uchar note, uchar velocity,
                       bigtime_t time);
  virtual void ProgramChange(uchar channel, uchar progNum, bigtime_t time);

  virtual void KeyPressure(uchar channel, uchar note, uchar pressure,
                           bigtime_t time);
  virtual void ControlChange(uchar channel, uchar controlNumber,
                             uchar controlValue, bigtime_t time);
  virtual void SystemExclusive(void *data, size_t dataLength, bigtime_t time);
  virtual void SystemCommon(uchar statusByte, uchar data1, uchar data2,
                            bigtime_t time);
  virtual void SystemRealTime(uchar statusByte, bigtime_t time);
  virtual void TempoChange(int32 bpm, bigtime_t time);

  virtual void AllNotesOff(bool justChannel, bigtime_t time);
};
#endif
