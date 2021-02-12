#include "ChordMenuItem.h"

ChordMenuItem::ChordMenuItem(const char *chord, const char *label,
                             BMessage *message, char shortcut, uint32 modifiers)
    :
    BMenuItem(label, message, shortcut, modifiers) {
  this->chord = chord;
}

const char *ChordMenuItem::Chord() { return chord.String(); }
