//
// C++ Implementation: cOutput
//
// Description: the cOutput class
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "coutput.h"

#include "cglobalsettings.h"
#include "cconsole.h"
#include "cprofilesettings.h"
#include "ctextchunk.h"

#include <kapplication.h>
#include <klocale.h>
#include <krandom.h>

cOutput::cOutput (int sess, QWidget *parent) : cActionBase ("output", sess)
{
  con = new cConsole (false, parent);
  con->setSession (sess);

  echocolor = Qt::yellow;
  systemcolor = Qt::cyan;
  bgcolor = Qt::black;

  // connect cConsole to us ...
  connect (con, SIGNAL (dimensionsChanged (int, int)), this,
      SLOT (dimensionsChanged (int, int)));
  connect (con, SIGNAL (sendCommand (const QString &)), this,
      SLOT (sendCommand (const QString &)));
  connect (con, SIGNAL (promptCommand (const QString &)), this,
      SLOT (promptCommand (const QString &)));

  // react on events
  addEventHandler ("display-line", 20, PT_TEXTCHUNK);
  addEventHandler ("display-prompt", 20, PT_TEXTCHUNK);
  addEventHandler ("command-sent", 50, PT_STRING);
  addEventHandler ("message", 50, PT_STRING);
  addEventHandler ("user-message", 50, PT_STRING);
  addGlobalEventHandler ("global-settings-changed", 50, PT_NOTHING);
}


cOutput::~cOutput ()
{
  removeEventHandler ("display-line");
  removeEventHandler ("display-prompt");
  removeEventHandler ("display-sent");
  removeEventHandler ("message");
  removeEventHandler ("user-message");
  removeGlobalEventHandler ("global-settings-changed");
}

void cOutput::eventNothingHandler (QString event, int session)
{
  if (event == "global-settings-changed") {
    cGlobalSettings *gs = cGlobalSettings::self();
    cConsole::setInitialHistorySize (gs->getInt ("history-size"));

    con->setFont (gs->getFont ("console-font"));
    setEchoColor (gs->getColor ("color-" + QString::number (gs->getInt ("echo-color"))));
    setSystemColor (gs->getColor ("color-" + QString::number (gs->getInt ("system-color"))));
    setEnableEcho (gs->getBool ("command-echo"));
    setEnableMessages (gs->getBool ("show-messages"));
    con->setEnableBlinking (gs->getBool ("allow-blink"));
    con->setWordWrapping (gs->getBool ("word-wrap"));
    con->setWrapPos (gs->getInt ("wrap-pos"));
    con->setIndentation (gs->getInt ("indent"));
    con->setRepaintCount (gs->getInt ("force-redraw"));
    con->tryUpdateHistorySize ();
  }
}

void cOutput::eventStringHandler (QString event, int, QString &par1, const QString &)
{
  if (event == "command-sent") {
    if (cmdecho)
      addCommand (par1);
  }
  else if (event == "message") {
    if (messages)  //only if messages are enabled
      systemMessage (par1);
  }
  else if (event == "user-message") {
    systemMessage (par1);
  }
}

void cOutput::eventChunkHandler (QString event, int, cTextChunk *chunk)
{
  if (event == "display-line")
    addLine (chunk);
  if (event == "display-prompt")
    addText (chunk);
}

void cOutput::dimensionsChanged (int x, int y)
{
  invokeEvent ("dimensions-changed", sess(), x, y);
}

void cOutput::sendCommand (const QString &command)
{
  // we send the command directly, with no parsing, because these commands
  // are coming from the server, and we want to keep some security ...
  invokeEvent ("send-command", sess(), command);
}

void cOutput::promptCommand (const QString &command)
{
  callAction ("inputline", "set-text", sess(), command);
}

void cOutput::setDefaultBkColor (QColor color)
{
  bgcolor = color;
  con->setDefaultBkColor (color);
}

void cOutput::setEchoColor (QColor color)
{
  echocolor = color;
}

void cOutput::setSystemColor (QColor color)
{
  systemcolor = color;
}

void cOutput::getAllColors (QColor &_echo, QColor &_system)
//this is used by cTranscript class to determine correct colors for command output
{
  _echo = echocolor;
  _system = systemcolor;
}

void cOutput::setEnableEcho (bool value)
{
  cmdecho = value;
}

void cOutput::setEnableMessages (bool value)
{
  messages = value;
}

void cOutput::addText (cTextChunk *chunk)
{
  cProfileSettings *sett = settings ();
  //prompt displayed if enabled in profile prefs, or if it's a quick-connection
  if ((!sett) || sett->getBool ("prompt-console"))
  {
    con->addText (chunk);
    invokeEvent ("displayed-prompt", sess(), chunk);
  }
}

void cOutput::addLine (cTextChunk *chunk)
{
  con->addLine (chunk);
  invokeEvent ("displayed-line", sess(), chunk);
}

void cOutput::addCommand (const QString &text)
//addText plus some colouring...
{
  //IMPORTANT: the signal emitted here is also captured by session transcript
  //so, if you disable cmd echo here, it won't work with session
  //transcript!!!

  cTextChunk *chunk = cTextChunk::makeLine (text, echocolor,
      bgcolor, con);
  addLine (chunk);
  delete chunk;
}

void cOutput::systemMessage (const QString &text)
//addText plus some colouring...
{
  con->forceBeginOfLine ();
  cTextChunk *chunk = cTextChunk::makeLine (text, systemcolor,
      bgcolor, con);
  addLine (chunk);
  delete chunk;
}

void cOutput::decisionMessage (const QString &text)
//addText plus some colouring...
{
  con->forceBeginOfLine ();
  cTextChunk *chunk = cTextChunk::makeLine (text, systemcolor,
      bgcolor, con);
  addLine (chunk);
  delete chunk;
}

void cOutput::makeDecision ()
{
  QString s;
  s = i18n ("My decision");
  s += ": ";
  QString ss;
  //generate a random number in 0..9 range
  int which = KRandom::random() % 10;
  switch (which) {
    case 0: ss = i18n ("No, no, no!"); break;
    case 1: ss = i18n ("I don't agree with it."); break;
    case 2: ss = i18n ("Better not."); break;
    case 3: ss = i18n ("I'd probably suggest to reject."); break;
    case 4: ss = i18n ("Saying no seems a bit better."); break;
    case 5: ss = i18n ("Saying yes seems a bit better."); break;
    case 6: ss = i18n ("I'd probably suggest to accept."); break;
    case 7: ss = i18n ("Sounds good."); break;
    case 8: ss = i18n ("I agree with it."); break;
    case 9: ss = i18n ("Definitely yes!"); break;
  };
  s += ss;
  //now display that decision
  decisionMessage (s);
}



#include "coutput.moc"