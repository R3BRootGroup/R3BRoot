##############################################################################
#   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2025-2026 Members of R3B Collaboration                     #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

generate_root_test_script(${R3BROOT_SOURCE_DIR}/glad/test/testGladSimulation.C)
add_test(NAME GladSimulation COMMAND ${R3BROOT_BINARY_DIR}/glad/test/testGladSimulation.sh)
set_tests_properties(GladSimulation PROPERTIES TIMEOUT "300")
set_tests_properties(GladSimulation PROPERTIES PASS_REGULAR_EXPRESSION
                                                  "Macro finished successfully.")

generate_root_test_script(${R3BROOT_SOURCE_DIR}/glad/test/testGladHecSimulation.C)
add_test(NAME GladHecSimulation COMMAND ${R3BROOT_BINARY_DIR}/glad/test/testGladHecSimulation.sh)
set_tests_properties(GladHecSimulation PROPERTIES TIMEOUT "300")
set_tests_properties(GladHecSimulation PROPERTIES PASS_REGULAR_EXPRESSION
                                                  "Macro finished successfully.")                                                  
