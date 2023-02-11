##############################################################################
#   Copyright (C) 2023 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2023 Members of R3B Collaboration                          #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

#!/bin/bash
set -e

source ${SIMPATH}/bin/geant4.sh
source ${SIMPATH}/bin/thisroot.sh

mkdir -p -v ../build
cd ../build
cmake ../R3BRoot -DUSE_DIFFERENT_COMPILER=TRUE
source config.sh
make -j8
