 ################################################################################
 #    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    #
 #                                                                              #
 #              This software is distributed under the terms of the             # 
 #         GNU Lesser General Public Licence version 3 (LGPL) version 3,        #  
 #                  copied verbatim in the file "LICENSE"                       #
 ################################################################################
MACRO (WRITE_ENV_CACHE filename)
  configure_file(${CMAKE_SOURCE_DIR}/cmake/scripts/env_cache.sh.in
	             ${CMAKE_CURRENT_BINARY_DIR}/${filename}
                )
ENDMACRO (WRITE_ENV_CACHE)
