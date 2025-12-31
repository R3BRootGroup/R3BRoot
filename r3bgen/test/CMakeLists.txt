##############################################################################
#   Copyright (C) 2020 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2020-2026 Members of R3B Collaboration                     #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

generate_root_test_script(${R3BROOT_SOURCE_DIR}/r3bgen/test/testR3BPhaseSpaceGeneratorIntegration.C)
add_test(NAME testR3BPhaseSpaceGeneratorIntegration COMMAND ${R3BROOT_BINARY_DIR}/r3bgen/test/testR3BPhaseSpaceGeneratorIntegration.sh)
set_tests_properties(testR3BPhaseSpaceGeneratorIntegration PROPERTIES TIMEOUT "100")
set_tests_properties(testR3BPhaseSpaceGeneratorIntegration PROPERTIES PASS_REGULAR_EXPRESSION "Macro finished successfully.")

generate_root_test_script(${R3BROOT_SOURCE_DIR}/r3bgen/test/testINCLRootGenerator.C)
add_test(NAME testINCLRootGenerator COMMAND ${R3BROOT_BINARY_DIR}/r3bgen/test/testINCLRootGenerator.sh)
set_tests_properties(testINCLRootGenerator PROPERTIES TIMEOUT "100")
set_tests_properties(testINCLRootGenerator PROPERTIES PASS_REGULAR_EXPRESSION "Macro finished successfully.")
