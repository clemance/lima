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
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/

#include "LinguisticProcessingClientFactory.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include <common/AbstractFactoryPattern/InitializableObjectFactory.h>
#include <iostream>
namespace Lima
{
  template<> MainFactory< RegistrableFactory< AbstractProcessingClientFactoryFactory > >* MainFactory< RegistrableFactory< AbstractProcessingClientFactoryFactory > >::s_instance(0);
  
  template<> MainFactory< RegistrableFactory< LinguisticProcessing::AbstractLinguisticProcessingClientFactory > >* MainFactory< RegistrableFactory< LinguisticProcessing::AbstractLinguisticProcessingClientFactory > >::s_instance(0);
  
namespace LinguisticProcessing
{

void LinguisticProcessingClientFactory::configureClientFactory(
  const std::string& id,
  Common::XMLConfigurationFiles::XMLConfigurationFileParser& configuration,
  std::deque<std::string> langs,
  std::deque<std::string> pipelines)
{
  LPCLIENTFACTORYLOGINIT;
  LINFO << "configure factory for client '" << id << "'";
  AbstractLinguisticProcessingClientFactory* factory=
    AbstractLinguisticProcessingClientFactory::getFactory(id);
  if (factory==0) {
    LERROR << "No factory defined for client '" << id << "' !";
    throw LinguisticProcessingException();
  }
  std::deque<std::string>::iterator Itrlangs;
//     for (  Itrlangs=langs.begin();  Itrlangs!=langs.end();Itrlangs++)
      //std::cout<<"ici la langue : "<<*Itrlangs<<std::endl;
  factory->configure(
    configuration,
    langs,
    pipelines);
}

AbstractProcessingClient* LinguisticProcessingClientFactory::createClient(
  const std::string& id) const 
{
  LPCLIENTFACTORYLOGINIT;
  LINFO << "Create client '" << id << "'";
  const AbstractLinguisticProcessingClientFactory* factory=
    AbstractLinguisticProcessingClientFactory::getFactory(id);
  if (factory==0) {
    LERROR << "No factory defined for client '" << id << "' !";
    throw LinguisticProcessingException();
  }
  
  return factory->createClient();
}


std::deque<std::string> LinguisticProcessingClientFactory::getRegisteredFactories() const
{
  return AbstractLinguisticProcessingClientFactory::getRegisteredFactories();
}


LinguisticProcessingClientFactoryFactory* LinguisticProcessingClientFactoryFactory::s_instance=new LinguisticProcessingClientFactoryFactory();

} // LinguisticProcessing

} // Lima
