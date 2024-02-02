################################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH      #
#   Copyright (C) 2019-2024 Members of R3B Collaboration                       #
#                                                                              #
#             This software is distributed under the terms of the              #
#                 GNU General Public Licence (GPL) version 3,                  #
#                    copied verbatim in the file "LICENSE".                    #
#                                                                              #
# In applying this license GSI does not waive the privileges and immunities    #
# granted to it by virtue of its status as an Intergovernmental Organization   #
# or submit itself to any jurisdiction.                                        #
################################################################################
 
MACRO (WRITE_ENV_CACHE filename)
  configure_file(${CMAKE_SOURCE_DIR}/cmake/scripts/env_cache.sh.in
	             ${CMAKE_CURRENT_BINARY_DIR}/${filename}
                )
ENDMACRO (WRITE_ENV_CACHE)
