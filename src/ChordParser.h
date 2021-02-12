#ifndef CHORD_PARSER_H
#define CHORD_PARSER_H

#include <File.h>
#include <Menu.h>
#include <stdio.h>

char NextChar(BFile &file);
char Skip(char ch, BFile &file);
char LoadChord(char ch, BFile &file, BMenu *menu);

#endif
