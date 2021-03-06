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
 *   Copyright (C) 2004-2012 by CEA LIST                      *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_ABSTRACTFACTORYPATTERN_REGISTRABLEFACTORY_H
#define LIMA_COMMON_ABSTRACTFACTORYPATTERN_REGISTRABLEFACTORY_H

#include "MainFactory.h"
#include <deque>
#include <string>

namespace Lima
{

/**
  * Defines a template for a factory that automatically register to a MainFactory. The registered
  * factories can be retreived by the static method getFactory
  * @brief defines a Factory that automatically register to a MainFactory singleton
  * @author Benoit Mathieu <mathieub@zoe.cea.fr>
  * @param Factory is the kind of Factories to register
  */
template<typename Factory>
class RegistrableFactory
{
public:

  /**
   * virtual destructor
   */
  virtual ~RegistrableFactory();

  /**
   * @brief get the factory corresponding to the given id
   * @param id Id of Factory to get
   * @return Factory*
   */
  static Factory* getFactory(const std::string& id);

  /**
   * @brief get the list of factories registered
   * @return deque of all ids of registered factories
   */
  static std::deque<std::string> getRegisteredFactories();

protected:
  /**
   * Constructor is protected, and must be redefined by subclass
   * @param id Id to use to register
   */
  RegistrableFactory(const std::string& id);

};

template<typename Factory>
RegistrableFactory<Factory>::RegistrableFactory(const std::string& id)
{
  //std::cerr << "RegistrableFactory<Factory>::RegistrableFactory("<< id << ")" << std::endl;
  MainFactory< RegistrableFactory<Factory> >::changeable().registerFactory(id,this);
}

template<typename Factory>
RegistrableFactory<Factory>::~RegistrableFactory() {}

template<typename Factory>
Factory* RegistrableFactory<Factory>::getFactory(const std::string& id)
{
  return static_cast<Factory*>(MainFactory< RegistrableFactory<Factory> >::changeable().getFactory(id));
}

template<typename Factory>
std::deque<std::string> RegistrableFactory<Factory>::getRegisteredFactories()
{
  return MainFactory< RegistrableFactory<Factory> >::single().getRegisteredFactories();
}

}

#endif
