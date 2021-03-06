/*
    Copyright 2002-2013 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/

#include "common/LimaCommon.h"
#include "common/Data/LimaString.h"

#include "common/Data/strwstrtools.h" 
#include "common/misc/AbstractAccessByString.h"
#include "common/FsaAccess/FsaAccessSpare16.h"

#include <QtCore/QCoreApplication>

using namespace std;
using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::Misc;
using namespace Lima::Common::FsaAccess;

// options
typedef struct ParamStruct
{
  std::string keyFileName;
  std::string prefix;
  int offset;
}
Param;

void testAccessMethod(const Param& param )
{
  string resourcesPath=string(getenv("LIMA_RESOURCES"));
  string commonConfigFile=string("lima-common.xml");
  string configDir=string(getenv("LIMA_CONF"));

  // Load lexicon
  Lima::Common::FsaAccess::FsaAccessSpare16* fsaAccess=new Lima::Common::FsaAccess::FsaAccessSpare16();
  fsaAccess->read(param.keyFileName);
  uint64_t size = fsaAccess->getSize();
  cerr <<  "FSA Access : " << size << " keys read from main keyfile" << endl;

  if( !param.prefix.empty() )
  {
    Lima::LimaString limaKey(Misc::utf8stdstring2limastring(param.prefix));
    Lima::LimaString extent = fsaAccess->getExtent(limaKey);
    std::cout << param.prefix << " => " << Misc::limastring2utf8stdstring(extent) << std::endl;
  }
}

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  QsLogging::initQsLog();
  
  setlocale(LC_ALL, "");

  // options reading
  Param param = {
                  std::string(),  // keyFileName
                  std::string(),  // prefix
                  -1,              // offset
                };

  for (int i = 1 ; i < argc; i++)
  {
    std::string arg(argv[i]);
    std::string::size_type pos = std::string::npos;
    if (arg == "--help")
    {
      std::cerr << "usage: " << argv[0]
      << " --help" << std::endl;
      std::cerr << "       " << argv[0]
      << " [--keyFileName=<string>]"
      << " [--prefix=<string>]"
      << " [--offset=<int>]"
      << std::endl;
      return 0;
    }
    else if ( (pos = arg.find("--keyFileName=")) != std::string::npos )
    {
      param.keyFileName = arg.substr(pos+14);
    }
    else if ( (pos = arg.find("--prefix=")) != std::string::npos )
    {
      param.prefix= arg.substr(pos+9);
    }
    else if ( (pos = arg.find("--offset=")) != std::string::npos )
    {
      param.offset = atoi( (arg.substr(pos+9)).c_str() );
    }
  }

  cerr << "testReadLexicon: ";
  if(param.keyFileName.size())
  {
    cerr << "--keyFileName='" << param.keyFileName << "' ";
  }
  if(param.prefix.size())
  {
    cerr << "--prefix='" << param.prefix << "' ";
  }
  cerr << "--offset="<< param.offset << " ";
  cerr << endl;
  
  testAccessMethod( param );
  return EXIT_SUCCESS;
}
