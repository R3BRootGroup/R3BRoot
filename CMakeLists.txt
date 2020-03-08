##############################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2019 Members of R3B Collaboration                          #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
cmake_policy(VERSION 3.12...3.16)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

              # VERSION aug19
project(R3BRoot VERSION 19.8.0 LANGUAGES CXX)


# Build options ##############################################################
option(WITH_UCESB "Build ..." OFF)
option(WITH_EPICS "Build ..." OFF)
option(WITH_ACTAR "Build ..." OFF)
option(WITH_SOFIA "Build ..." OFF)
option(WITH_GTPC "Build ..." OFF)
option(WITH_FRS "Build ..." OFF)
##############################################################################


# Dependencies ###############################################################
include(FairRootFindPackage2) # load local copy

find_package2(PUBLIC FairRoot REQUIRED
  VERSION 18.2.0
  COMPONENTS sim mbs
  OPTIONAL_COMPONENTS mq eventdisplay
  FIND_DEPENDENCIES)

include(FairRootChecks)
fairroot_sanity_checks()
include(FairRootFindPackage2) # reload from FairRoot installation
include(R3BMacros)

#find_package2(PUBLIC CLHEP)
find_package2(PUBLIC HEPMC)

find_package2(PUBLIC ucesb)
if(ucesb_FOUND)
    set(WITH_UCESB true)
    set(LD_LIBRARY_PATH ${ucesb_LIBRARY_DIR} ${LD_LIBRARY_PATH})
else()
  # message(WARNING "${BMagenta}No UCESB configured. You will not be able to unpack lmd files. Set UCESB_DIR"
    # " to a ucesb installation to fix this.${CR}")
endif(ucesb_FOUND)

find_package2(PUBLIC EPICS)
if(EPICS_FOUND)
  set(WITH_EPICS true)
  set(LD_LIBRARY_PATH ${EPICS_LIBRARY_DIR} ${LD_LIBRARY_PATH})
endif(EPICS_FOUND)

find_package2(PUBLIC Atima)

find_package2(PUBLIC Garfield)
if(GARFIELD_FOUND)
  set(WITH_ACTAR true)
  set(LD_LIBRARY_PATH ${GARFIELD_LIBRARY_DIR} ${LD_LIBRARY_PATH})
endif(GARFIELD_FOUND)

if(EXISTS "${R3BROOT_SOURCE_DIR}/sofia/")
  message("Building SOFIA framework...")
  set(WITH_SOFIA true)
  add_definitions(-DSOFIA)
endif(EXISTS "${R3BROOT_SOURCE_DIR}/sofia/")

if(EXISTS "${R3BROOT_SOURCE_DIR}/glad-tpc/")
  message("Building GLAD-TPC detector...")
  set(WITH_GTPC true)
  add_definitions(-DGTPC)
endif(EXISTS "${R3BROOT_SOURCE_DIR}/glad-tpc/")

if(EXISTS "${R3BROOT_SOURCE_DIR}/frs/")
  message("Building FRS framework...")
  set(WITH_FRS true)
  add_definitions(-DFRS)
endif(EXISTS "${R3BROOT_SOURCE_DIR}/frs/")
##############################################################################


# Targets ####################################################################
# TODO
# set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
# set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
# set(INCLUDE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/include")
# set(VMCWORKDIR ${CMAKE_SOURCE_DIR})
#
# SetBasicVariables()
#
# r3b_generate_version_info()
#
include(CTest)
include(FairRootTargetRootDictionary) # helper function to generate root dicts
include(FairRootUtils) # helper function to guess header files

add_subdirectory(r3bbase)
# if(WITH_UCESB)
  # add_subdirectory(r3bsource)
# endif(WITH_UCESB)
# if(WITH_EPICS)
  # add_subdirectory (epics)
# endif()
# if(Atima_FOUND)
  # add_subdirectory (atima)
# endif()
# add_subdirectory(passive)
# add_subdirectory(califa)
# add_subdirectory(xball)
# add_subdirectory(dch)
# add_subdirectory(gfi)
# add_subdirectory(fiber)
# add_subdirectory(fi0)
# add_subdirectory(fi1a)
# add_subdirectory(fi1b)
# add_subdirectory(fi2a)
# add_subdirectory(fi2b)
# add_subdirectory(fi3a)
# add_subdirectory(fi3b)
# add_subdirectory(fi4)
# add_subdirectory(fi5)
# add_subdirectory(fi6)
# add_subdirectory(fi7)
# add_subdirectory(fi8)
# add_subdirectory(fi9)
# add_subdirectory(fi10)
# add_subdirectory(fi11)
# add_subdirectory(fi12)
# add_subdirectory(fi13)
# add_subdirectory(sfi)
# add_subdirectory(land)
# add_subdirectory(neuland)
# add_subdirectory(los)
# add_subdirectory(rolu)
# add_subdirectory(sci2)
# add_subdirectory(sci8)
# add_subdirectory(mtof)
# add_subdirectory(tof)
# add_subdirectory(tracker)
# add_subdirectory(startrack)
# add_subdirectory(strawtubes)
# add_subdirectory(music)
# add_subdirectory(r3bdata)
# add_subdirectory(field)
# add_subdirectory(macros)
# add_subdirectory(r3bgen)
# add_subdirectory(lumon)
# add_subdirectory(evtvis)
# add_subdirectory(mfi)
# add_subdirectory(psp)
# add_subdirectory(timestitcher)
# add_subdirectory(compilehelper)
# add_subdirectory(tcal)
# add_subdirectory(tracking)
# add_subdirectory(tracker_rene)
# if(WITH_ACTAR)
  # add_subdirectory(actar)
# endif()
# if(WITH_SOFIA)
  # add_subdirectory(sofia)
# endif()
# if(WITH_GTPC)
  # add_subdirectory(glad-tpc)
# endif()
# if(WITH_FRS)
  # add_subdirectory(frs)
# endif()

# TODO
# if(RULE_CHECKER_FOUND)
#
    # add_custom_target(RULES
                      # COMMAND ${RULE_CHECKER_SCRIPT1} ${CMAKE_BINARY_DIR} viol > violations.html
                      # DEPENDS $ENV{ALL_RULES})
#
# endif(RULE_CHECKER_FOUND)
##############################################################################


# Installation ###############################################################
#
# if(NOT R3BROOT_MINIMAL)
    # write_config_file(config.sh)
    # write_config_file(config.csh)
# else(NOT R3BROOT_MINIMAL)
    # write_minimal_config_file(config.sh)
# endif(NOT R3BROOT_MINIMAL)
#
# write_env_cache(env_cache.sh)
##############################################################################

r3broot_summary()
