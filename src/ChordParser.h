/*
 * Copyright 2000-2013. All rights reserved.
 * Distributed under the terms of the GPLv2 license.
 *
 * Author:
 *	Michael Pfeiffer et al.
 *
 */

#ifndef CHORD_PARSER_H
#define CHORD_PARSER_H

#include <File.h>
#include <Menu.h>
#include <stdio.h>

char NextChar(BFile& file);
char Skip(char ch, BFile& file);
char LoadChord(char ch, BFile& file, BMenu* menu);

#endif
