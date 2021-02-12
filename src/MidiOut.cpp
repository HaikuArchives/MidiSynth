#include "MidiOut.h"

MidiOut::MidiOut(const char *name)
    :
    BMidiLocalProducer(name) {}
/*
void MidiOut::ChannelPressure(uchar channel, uchar pressure, bigtime_t time) {
        SprayChannelPressure(channel, pressure, time);
}
void MidiOut::PitchBend(uchar channel, uchar lsb, uchar msb, bigtime_t time) {
        SprayPitchBend(channel, lsb, msb, time);
}
void MidiOut::NoteOn(uchar channel, uchar note, uchar velocity, bigtime_t time)
{ SprayNoteOn(channel, note, velocity, time);
}
void MidiOut::NoteOff(uchar channel, uchar note, uchar velocity, bigtime_t time)
{ SprayNoteOff(channel, note, velocity, time);
}
void MidiOut::ProgramChange(uchar channel, uchar progNum, bigtime_t time) {
        SprayProgramChange(channel, progNum, time);
}
*/
void MidiOut::AllNotesOff(bool justChannel, bigtime_t time) {
  for (uchar ch = 0; ch < 16; ch++) {
    SprayControlChange(ch, B_ALL_NOTES_OFF, 0, time);
    SprayControlChange(ch, B_PAN, 63, time);
    SprayChannelPressure(ch, 0xff, time);
  }
}
