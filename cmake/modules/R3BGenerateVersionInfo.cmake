 ################################################################################
 #    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    #
 #                                                                              #
 #              This software is distributed under the terms of the             # 
 #         GNU Lesser General Public Licence version 3 (LGPL) version 3,        #  
 #                  copied verbatim in the file "LICENSE"                       #
 ################################################################################
 # Configure FairVersion.h
 # ------------------------------

 Find_Package(Git)

 If(GIT_FOUND AND EXISTS "${SOURCE_DIR}/.git")
   Execute_Process(COMMAND ${GIT_EXECUTABLE} describe
                   OUTPUT_VARIABLE R3BROOT_GIT_VERSION
                   OUTPUT_STRIP_TRAILING_WHITESPACE
                   WORKING_DIRECTORY ${SOURCE_DIR}
                  )
   Execute_Process(COMMAND ${GIT_EXECUTABLE} log -1 --format=%cd
                   OUTPUT_VARIABLE R3BROOT_GIT_DATE
                   OUTPUT_STRIP_TRAILING_WHITESPACE
                   WORKING_DIRECTORY ${SOURCE_DIR}
                  )
   Message(STATUS "R3BRoot Version - ${R3BROOT_GIT_VERSION} from - ${R3BROOT_GIT_DATE}")
   Configure_File(${SOURCE_DIR}/cmake/scripts/R3BRootVersion.h.tmp ${BINARY_DIR}/R3BRootVersion.h @ONLY)
  
 Else()
   Configure_File(${SOURCE_DIR}/cmake/scripts/R3BRootVersion.h.default ${BINARY_DIR}/R3BRootVersion.h COPYONLY)
 EndIf()

