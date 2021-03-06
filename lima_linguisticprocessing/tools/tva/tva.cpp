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
 *   Copyright (C) 2004 by Benoit Mathieu                                  *
 *   mathieub@zoe.cea.fr                                                   *
 *                                                                         *
 ***************************************************************************/

#include "linguisticProcessing/common/tgv/TestCasesHandler.h"
#include "tools/tva/AnalysisTestCase.h"
#include "linguisticProcessing/common/BagOfWords/bowXMLWriter.h"

#include <iostream>

#include "common/LimaCommon.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/Handler/AbstractAnalysisHandler.h"

#include "linguisticProcessing/client/AbstractLinguisticProcessingClient.h"
#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/BowTextWriter.h"

#include <QtCore/QCoreApplication>
#include <QtXml/QXmlSimpleReader>
// #ifdef WIN32
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
// #endif

using namespace Lima::Common::TGV;
using namespace Lima::AnalysisValidation;
using namespace Lima::LinguisticProcessing;
using namespace Lima::Common::MediaticData;
using namespace Lima;


void usage(int argc, char* argv[]);

int main(int argc,char* argv[])
{
  QCoreApplication a(argc, argv);
  QsLogging::initQsLog();
// #ifdef WIN32
  // Necessary to initialize factories under Windows
  Lima::AmosePluginsManager::single();
// #endif
  std::string resourcesPath=std::string(getenv("LIMA_RESOURCES"));
  std::string configDir=std::string(getenv("LIMA_CONF"));
  std::string lpConfigFile=std::string("lima-lp-tva.xml");
  std::string commonConfigFile=std::string("lima-common.xml");
  std::string clientId=std::string("lima-coreclient");
  std::string workingDir=std::string(".");

  std::deque<std::string> files;
  std::deque<std::string> pipelines;
  std::deque<std::string> langs;

  if (argc>1)
  {
    for (int i = 1 ; i < argc; i++)
    {
      std::string arg(argv[i]);
      std::string::size_type pos = std::string::npos;
      if ( arg[0] == '-' )
      {
        if (arg == "--help")
          usage(argc, argv);
        else if ( (pos = arg.find("--lp-config-file=")) != std::string::npos )
          lpConfigFile = arg.substr(pos+17);
        else if ( (pos = arg.find("--common-config-file=")) != std::string::npos )
          commonConfigFile = arg.substr(pos+21);
        else if ( (pos = arg.find("--config-dir=")) != std::string::npos )
          configDir = arg.substr(pos+13);
        else if ( (pos = arg.find("--resources-dir=")) != std::string::npos )
          resourcesPath = arg.substr(pos+16);
        else if ( (pos = arg.find("--client=")) != std::string::npos )
          clientId=arg.substr(pos+9);
        else if ( (pos = arg.find("--working-dir=")) != std::string::npos )
          workingDir=arg.substr(pos+14);
        else if ( (pos = arg.find("--language=")) != std::string::npos )
          langs.push_back(arg.substr(pos+11));
        else usage(argc, argv);
      }
      else
      {
        files.push_back(arg);
      }
    }
  }

  if(langs.empty()) {
    std::cerr << "No language specified. Aborting." << std::endl;
    return 1;
  }
    
  setlocale(LC_ALL,"fr_FR.UTF-8");

  AbstractLinguisticProcessingClient* client(0);

  // initialize common
  MediaticData::changeable().init(
    resourcesPath,
    configDir,
    commonConfigFile,
    langs);

  // initialize linguistic processing
  Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(configDir + "/" + lpConfigFile);
  LinguisticProcessingClientFactory::changeable().configureClientFactory(
    clientId,
    lpconfig,
    langs,
    pipelines);

  client=static_cast<AbstractLinguisticProcessingClient*>(LinguisticProcessingClientFactory::single().createClient(clientId));

  // Set the handlers
  std::map<std::string, AbstractAnalysisHandler*> handlers;
  BowTextWriter* bowTextWriter = new BowTextWriter();
  handlers.insert(std::make_pair("bowTextWriter", bowTextWriter));
  SimpleStreamHandler* simpleStreamHandler = new SimpleStreamHandler();
  handlers.insert(std::make_pair("simpleStreamHandler", simpleStreamHandler));
  BowTextHandler* bowTextHandler = new BowTextHandler();
  handlers.insert(std::make_pair("bowTextHandler", bowTextHandler));

  AnalysisTestCaseProcessor analysisTestCaseProcessor(workingDir, client, handlers);
    
  QXmlSimpleReader parser;
  TestCasesHandler tch(analysisTestCaseProcessor);

  parser.setContentHandler(&tch);
  parser.setErrorHandler(&tch);

  for (std::deque<std::string>::const_iterator it=files.begin();
       it!=files.end();
       it++)
  {
    std::cout << "process tests in " << *it << std::endl;
    try
    {
      QFile file(it->c_str());
      if (!file.open(QIODevice::ReadOnly))
      {
        std::cerr << "Error opening " << *it << std::endl;
        return 1;
      }
      if (!parser.parse( QXmlInputSource(&file)))
      {
        std::cerr << "Error parsing " << *it << " : " << parser.errorHandler()->errorString().toUtf8().constData() << std::endl;
        return 1;
      }
    }
    catch (Lima::LimaException& e)
    {
      std::cerr << __FILE__ << ", line " << __LINE__ << ": caught LimaException : " << std::endl << e.what() << std::endl;
    }
    catch (std::logic_error& e)
    {
      std::cerr << __FILE__ << ", line " << __LINE__ << ": caught logic_error : " << std::endl << e.what() << std::endl;
    }
    catch (std::runtime_error& e)
    {
      std::cerr << __FILE__ << ", line " << __LINE__ << ": caught runtime_error : " << std::endl << e.what() << std::endl;
    }

    TestCasesHandler::TestReport resTotal;
    std::cout << std::endl;
    std::cout << "=========================================================" << std::endl;
    std::cout << std::endl;
    std::cout << "  TestReport :   " << *it << " " << std::endl;
    std::cout << std::endl;
    std::cout << "\ttype           \tsuccess\tcond.\tfailed\ttotal" << std::endl;
    std::cout << "---------------------------------------------------------" << std::endl;
    for (std::map<std::string,TestCasesHandler::TestReport>::const_iterator resItr=tch.m_reportByType.begin();
         resItr!=tch.m_reportByType.end();
         resItr++)
    {
      std::string label(resItr->first);
      label.resize(15,' ');
      std::cout << "\t" << label << "\t" << resItr->second.success << "\t" << resItr->second.conditional << "\t" << resItr->second.failed << "\t" << resItr->second.nbtests << std::endl;
      resTotal.success+=resItr->second.success;
      resTotal.conditional+=resItr->second.conditional;
      resTotal.failed+=resItr->second.failed;
      resTotal.nbtests+=resItr->second.nbtests;
    }
    std::cout << "---------------------------------------------------------" << std::endl;
    std::cout << "\ttotal          \t" << resTotal.success << "\t" << resTotal.conditional << "\t" << resTotal.failed << "\t" << resTotal.nbtests << std::endl;
    std::cout << "=========================================================" << std::endl;
    std::cout << std::endl;
    tch.m_reportByType.clear();
  }
  delete client;
  delete bowTextWriter;
  delete simpleStreamHandler;
  delete bowTextHandler;
}


void usage(int argc, char *argv[])
{
  LIMA_UNUSED(argc);
  std::cout << "usage: " << argv[0] << " [OPTIONS] [file1 [file2 [...]]] " << std::endl;
  std::cout << "\t--working-dir=</path/to/the/working/dir> Optional. Default is ./" << std::endl;
  std::cout << "\t--resources-dir=</path/to/the/resources> Optional. Default is $LIMA_RESOURCES" << std::endl;
  std::cout << "\t--config-dir=</path/to/the/configuration/directory> Optional. Default is $LIMA_CONF" << std::endl;
  std::cout << "\t--lp-config-file=<configuration/file/name>\tOptional. Default is lima-lp-tva.xml" << std::endl;
  std::cout << "\t--common-config-file=<configuration/file/name>\tOptional. Default is lima-common.xml" << std::endl;
  std::cout << "\t--client=<clientId>\tOptional. Default is 'lima-coreclient'" << std::endl;
  std::cout << "\t--pipeline=<pipelineId>\tOptional. Default is 'main'" << std::endl;
  std::cout << "\t--language=<lang>\tOptional. Default initialize all available languages in common config file" << std::endl;
  std::cout << "\twhere files are files to analyze." << std::endl;
  std::cout << std::endl;
  std::cout << "Available client factories are : " << std::endl;
  {
    std::deque<std::string> ids=LinguisticProcessingClientFactory::single().getRegisteredFactories();
    for (std::deque<std::string>::iterator it=ids.begin();
         it!=ids.end();
         it++)
    {
      std::cout << "- " << *it << std::endl;
    }
    std::cout << std::endl;
  }
  exit(0);
}
