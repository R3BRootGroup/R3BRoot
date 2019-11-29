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

generate_root_test_script(${R3BROOT_SOURCE_DIR}/neuland/test/calibration/testNeulandQcal.C)
add_test(NeulandQcal ${R3BROOT_BINARY_DIR}/neuland/test/calibration/testNeulandQcal.sh)
set_tests_properties(NeulandQcal PROPERTIES TIMEOUT "100")
set_tests_properties(NeulandQcal PROPERTIES PASS_REGULAR_EXPRESSION "Test successful!")

generate_root_test_script(${R3BROOT_SOURCE_DIR}/neuland/test/calibration/testNeulandTcal.C)
add_test(NeulandTcal ${R3BROOT_BINARY_DIR}/neuland/test/calibration/testNeulandTcal.sh)
set_tests_properties(NeulandTcal PROPERTIES TIMEOUT "100")
set_tests_properties(NeulandTcal PROPERTIES PASS_REGULAR_EXPRESSION "Test successful!")

#generate_root_test_script(${R3BROOT_SOURCE_DIR}/neuland/test/calibration/testNeulandCosmic1.C)
#add_test(NeulandCosmic1 ${R3BROOT_BINARY_DIR}/neuland/test/calibration/testNeulandCosmic1.sh)
#set_tests_properties(NeulandCosmic1 PROPERTIES TIMEOUT "3600")
#set_tests_properties(NeulandCosmic1 PROPERTIES PASS_REGULAR_EXPRESSION "Test successful!")
