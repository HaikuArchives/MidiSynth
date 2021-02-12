/* ===================================================================== *
 * InternalSynth.h
 * ---------------------------------------------------------------------
 * Purpose:
 *  Midi2 wrapper for the midisynth.
 * ---------------------------------------------------------------------
 * History:
 *	5/19/2000 dwalton
 *		Original implementation
 * ---------------------------------------------------------------------
 * To Do:
 *
 * ===================================================================== */

#ifndef __C_InternalSynth_H__
#define __C_InternalSynth_H__

// Midi Kit
#include <MidiSynth.h>
// Midi Kit 2
#include <MidiConsumer.h>
// Storage Kit
#include <Mime.h>

class CInternalSynth : public BMidiLocalConsumer {

public: // Constructor/Destructor
  CInternalSynth(char *name);

  ~CInternalSynth();

public: // Accessors
  BBitmap *GetIcon(icon_size size) const;

public: // Operations
  void Init();

  void SetVolume(double volume) { m_midiSynth->SetVolume(volume); }

  void ProgramChange(int channel, int inst) {
    m_midiSynth->ProgramChange(channel, inst);
  }

  bool LoadSynthData(entry_ref *ref);
  bool LoadSynthData(void);

public: // BMidiLocalConsumer Implementation
  virtual void NoteOff(uchar channel, uchar note, uchar velocity,
                       bigtime_t time);

  virtual void NoteOn(uchar channel, uchar note, uchar velocity,
                      bigtime_t time);

  virtual void KeyPressure(uchar channel, uchar note, uchar pressure,
                           bigtime_t time);

  virtual void ControlChange(uchar channel, uchar controlNumber,
                             uchar controlValue, bigtime_t time);

  virtual void ProgramChange(uchar channel, uchar programNumber,
                             bigtime_t time);

  virtual void ChannelPressure(uchar channel, uchar pressure, bigtime_t time);

  virtual void PitchBend(uchar channel, uchar lsb, uchar msb, bigtime_t time);

private: // Instance Data
  BMidiSynth *m_midiSynth;
};

#endif /*__C_InternalSynth_H__ */
