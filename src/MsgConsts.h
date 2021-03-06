/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#ifndef MSG_CONSTS_H
#define MSG_CONSTS_H

#define APPLICATION "MidiSynth"
#define VERSION "1.8 (Haiku)"
#define SIGNATURE "application/x-vnd.midisynth"

#define MSG_INVALIDATE 1000
#define MSG_SCOPE_CLOSED_NOTIFY 'SClN'

#define MENU_APP_NEW 'APnw'
#define MENU_KEYBOARD_2D 'KB2D'
#define MENU_KEYBOARD_3D 'KB3D'
#define MENU_PATCH_SELECTED 'PSel'
#define MENU_REMAP_KEYS 'MPKS'
#define MENU_KEY_MAP_SELECTED 'KMSl'

#define MENU_SCOPE 'SCOP'
#define MENU_RESET 'RST'
#define MENU_REVERB 'RVRB'
#define MENU_SAMPLING_RATE 'SRAT'
#define MENU_INTERPOLATION 'INTP'
#define MENU_MAX_SYNTH_VOICES 'MSVS'
#define MENU_LIMITER_THRESHHOLD 'LTHD'
#define MENU_KEYBOARD_OCTAVES 'KBOC'
#define MENU_KEYBOARD_ROWS 'KBRS'

#define MENU_GROUPS 100 // to 115
#define MENU_OCTAVE 120 // to 132
#define MENU_CHANNEL 180 // to 195
#define MENU_INSTRUMENTS 200 // to 328
#define MENU_SYNTH_ENABLED 340
#define MENU_CHORD 341
#define MENU_PORT 400 // to 463
#define MENU_OUTPORT 'MOUT'
#define MENU_INPORT 'MINN'
#define MSG_PITCH_BEND_CHANGED 'PBCH'
#define MSG_VOLUME_CHANGED 'VOCH'
#define MSG_VELOCITY_CHANGED 'VECH'
#define MSG_CH_PRESSURE_CHANGED 'CPCH'
#define MSG_PAN_CHANGED 'PNCH'

#endif
