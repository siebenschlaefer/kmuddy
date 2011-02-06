//
// C++ Implementation: stringsplugin
//
// Description: Strings plugin.
//
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "stringsplugin.h"

#include "cmacromanager.h"

#include <kpluginfactory.h>
#include <kpluginloader.h>

K_PLUGIN_FACTORY (cStringsPluginFactory, registerPlugin<cStringsPlugin>();)
K_EXPORT_PLUGIN (cStringsPluginFactory("kmuddy"))

    
// all function implementations start here:

class cFunctionLower: public cFunction {
  public:
    cFunctionLower () : cFunction ("lower") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 1) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      QString ret = (*it).asString().toLower();
      return cValue (ret);
    }
};

class cFunctionUpper: public cFunction {
  public:
    cFunctionUpper () : cFunction ("upper") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 1) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      QString ret = (*it).asString().toUpper();
      return cValue (ret);
    }
};

class cFunctionCaps: public cFunction {
  public:
    cFunctionCaps () : cFunction ("caps") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 1) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      QString ret = (*it).asString().toLower();
      bool wasws = true;
      for (int i = 0; i < ret.length(); ++i)
        if (!ret[i].isLetterOrNumber())
          wasws = true;
        else
          if (wasws) {
            ret[i] = ret[i].toUpper();
            wasws = false;
          }
      return cValue (ret);
    }
};

class cFunctionLeft: public cFunction {
  public:
    cFunctionLeft () : cFunction ("left") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 1) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      QString ret = (*it).asString();
      if (params.size() < 2) return ret;
      ++it;
      int chars = (*it).asInteger();
      ret = ret.left (chars);
      return cValue (ret);
    }
};

class cFunctionRight: public cFunction {
  public:
    cFunctionRight () : cFunction ("right") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 1) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      QString ret = (*it).asString();
      if (params.size() < 2) return ret;
      ++it;
      int chars = (*it).asInteger();
      ret = ret.right (chars);
      return cValue (ret);
    }
};

class cFunctionMid: public cFunction {
  public:
    cFunctionMid () : cFunction ("mid") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 1) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      QString ret = (*it).asString();
      if (params.size() < 2) return ret;
      ++it;
      int chars = (*it).asInteger();
      if (params.size() >= 3) {
        ++it;
        int len = (*it).asInteger();
        ret = ret.mid (chars, len);
      } else
        ret = ret.mid (chars);
      return cValue (ret);
    }
};

class cFunctionStrlen: public cFunction {
  public:
    cFunctionStrlen () : cFunction ("strlen") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 1) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      int ret = (*it).asString().length();
      return cValue (ret);
    }
};

class cFunctionTrim: public cFunction {
  public:
    cFunctionTrim () : cFunction ("trim") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 1) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      QString ret = (*it).asString().trimmed();
      return cValue (ret);
    }
};

class cFunctionStrpos: public cFunction {
  public:
    cFunctionStrpos () : cFunction ("strpos") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 2) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      QString haystack = (*it).asString();
      ++it;
      QString needle = (*it).asString();
      return cValue (haystack.indexOf (needle));
    }
};

class cFunctionStrfill: public cFunction {
  public:
    cFunctionStrfill () : cFunction ("strfill") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 1) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      QString ret = (*it).asString();
      if (params.size() < 3) return ret;
      ++it;
      QChar character = (*it).asString()[0];
      ++it;
      int len = (*it).asInteger();
      
      if (len <= ret.length())
        return cValue (ret);
      
      QString filler;
      filler.fill (character, len - ret.length());
      return cValue (ret + filler);
    }
};

class cFunctionStrleftfill: public cFunction {
  public:
    cFunctionStrleftfill () : cFunction ("strleftfill") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      // same as strfill, except for the last line
      if (params.size() < 1) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      QString ret = (*it).asString();
      if (params.size() < 3) return ret;
      ++it;
      QChar character = (*it).asString()[0];
      ++it;
      int len = (*it).asInteger();
      
      if (len <= ret.length())
        return cValue (ret);
      
      QString filler;
      filler.fill (character, len - ret.length());
      return cValue (filler + ret);
    }
};

class cFunctionStrcat: public cFunction {
  public:
    cFunctionStrcat () : cFunction ("strcat") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 1) return cValue::empty();
      std::list<cValue>::iterator it;
      QString ret;
      for (it = params.begin(); it != params.end(); ++it)
        ret += (*it).asString();
      return cValue (ret);
    }
};

class cFunctionJoin: public cFunction {
  public:
    cFunctionJoin () : cFunction ("join") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 1) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      cValue list = *it;
      QString sep = "|";
      if (params.size() >= 2)
      {
        ++it;
        sep = (*it).asString();
      }
      return list.listJoin (sep);
    }
};

class cFunctionExplode: public cFunction {
  public:
    cFunctionExplode () : cFunction ("explode") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 1) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      cValue str = *it;
      QString sep = "|";
      if (params.size() >= 2)
      {
        ++it;
        sep = (*it).asString();
      }
      return str.toList (sep);
    }
};

class cFunctionReplace: public cFunction {
  public:
    cFunctionReplace () : cFunction ("replace") {}
    virtual cValue eval (std::list<cValue> &params, int, cCmdQueue *) {
      if (params.size() < 3) return cValue::empty();
      std::list<cValue>::iterator it = params.begin();
      QString str = (*it).asString();
      ++it;
      QString from = (*it).asString();
      ++it;
      QString to = (*it).asString();
      str.replace (from, to);
      return cValue (str);
    }
};


// the main plug-in code which registers the functions:

struct cStringsPluginPrivate {
  cFunctionLower *flower;
  cFunctionUpper *fupper;
  cFunctionCaps *fcaps;
  cFunctionLeft *fleft;
  cFunctionRight *fright;
  cFunctionMid *fmid;
  cFunctionStrlen *fstrlen;
  cFunctionTrim *ftrim;
  cFunctionStrpos *fstrpos;
  cFunctionStrfill *fstrfill;
  cFunctionStrleftfill *fstrleftfill;
  cFunctionStrcat *fstrcat;
  cFunctionJoin *fjoin;
  cFunctionExplode *fexplode;
  cFunctionReplace *freplace;
};

cStringsPlugin::cStringsPlugin (QObject *, const QVariantList &)
{
  d = new cStringsPluginPrivate;
  registerFunctions ();
}

cStringsPlugin::~cStringsPlugin()
{
  unregisterFunctions ();
  delete d;
}

void cStringsPlugin::registerFunctions ()
{
  d->flower = new cFunctionLower;
  d->fupper = new cFunctionUpper;
  d->fcaps = new cFunctionCaps;
  d->fleft = new cFunctionLeft;
  d->fright = new cFunctionRight;
  d->fmid = new cFunctionMid;
  d->fstrlen = new cFunctionStrlen;
  d->ftrim = new cFunctionTrim;
  d->fstrpos = new cFunctionStrpos;
  d->fstrfill = new cFunctionStrfill;
  d->fstrleftfill = new cFunctionStrleftfill;
  d->fstrcat = new cFunctionStrcat;
  d->fjoin = new cFunctionJoin;
  d->fexplode = new cFunctionExplode;
  d->freplace = new cFunctionReplace;
}

void cStringsPlugin::unregisterFunctions ()
{
  delete d->flower;
  delete d->fupper;
  delete d->fcaps;
  delete d->fleft;
  delete d->fright;
  delete d->fmid;
  delete d->fstrlen;
  delete d->ftrim;
  delete d->fstrpos;
  delete d->fstrfill;
  delete d->fstrleftfill;
  delete d->fstrcat;
  delete d->fjoin;
  delete d->fexplode;
  delete d->freplace;
}


