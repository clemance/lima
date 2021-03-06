#   Copyright 2002-2013 CEA LIST
#    
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/src )

add_subdirectory(AnalysisHandlers)

add_definitions(-DLIMA_LINGUISTICPROCESSIONGCLIENT_EXPORTING)

########### next target ###############

SET(lima-lp-client_LIB_SRCS
   LinguisticProcessingClientFactory.cpp
)

add_library(lima-lp-client SHARED ${lima-lp-client_LIB_SRCS})

target_link_libraries(lima-lp-client lima-common-factory lima-common-qslog ${QT_LIBRARIES})

set_target_properties(lima-lp-client PROPERTIES VERSION ${LIMA_LP_LIB_VERSION} SOVERSION ${LIMA_LP_LIB_SOVERSION})

install(TARGETS lima-lp-client DESTINATION lib)


########### install files ###############
install(FILES
    LinguisticProcessingClientExport.h
    AbstractLinguisticProcessingClient.h 
    LinguisticProcessingException.h
    LinguisticProcessingClientFactory.h
  DESTINATION include/linguisticProcessing/client)
