#include "ChordParser.h"
#include "ChordMenuItem.h"
#include "MsgConsts.h"
#include <String.h>
#include <ctype.h>

char NextChar(BFile &file) {
  char ch;
  if (file.Read(&ch, sizeof(ch)) <= 0)
    return EOF;
  else
    return ch;
}

static char SkipComment(BFile &file) {
  char ch;
  while (((ch = NextChar(file)) != EOF) && (ch != '\n'))
    ;
  return ch;
}

static char SkipWhiteSpaces(BFile &file) {
  char ch;
  while (isspace(ch = NextChar(file)) && (ch != EOF))
    ;
  return ch;
}

char Skip(char ch, BFile &file) {
  while ((ch == '#') || isspace(ch)) {
    if (ch == '#')
      ch = SkipComment(file);
    else
      ch = SkipWhiteSpaces(file);
  }
  return ch;
}

char LoadChord(char ch, BFile &file, BMenu *menu) {
  BString c, n;
  if (!isalnum(ch))
    return EOF;

  while (isalnum(ch)) {
    c += ch;
    ch = NextChar(file);
  }

  ch = Skip(ch, file);

  if (ch != '"')
    return EOF;
  ch = NextChar(file);

  while ((ch != '"') && (ch != '\n') && (ch != EOF)) {
    n += ch;
    ch = NextChar(file);
  }
  if (ch != '"')
    return EOF;
  ch = NextChar(file);

  menu->AddItem(
      new ChordMenuItem(c.String(), n.String(), new BMessage(MENU_CHORD)));
  return ch;
}
