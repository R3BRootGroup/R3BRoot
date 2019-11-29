/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// Configuration macro for Geant3 VirtualMC

void Config()
{
    cout << "[g3Config]: Creating TGeant3 VirtualMC ..." << endl;

    auto geant3 = new TGeant3TGeo("C++ Interface to Geant3");

    auto stack = new R3BStack();
    stack->SetDebug(kFALSE);
    stack->StoreSecondaries(kTRUE);
    stack->SetMinPoints(1);
    geant3->SetStack(stack);

    // Geant3 specific configuration for simulated Runs
    geant3->SetTRIG(1);       // Number of events to be processed
    geant3->SetSWIT(4, 10);   // (2, 2) for debugging
    geant3->SetDEBU(0, 0, 1); // (1, 100, 2) for debugging

    // Other Geant3 settings
    geant3->SetAUTO(1); // Select automatic STMIN etc... calc. (AUTO 1) or manual (AUTO 0)
    geant3->SetABAN(0); // Restore 3.16 behaviour for abandoned tracks
    geant3->SetOPTI(2); // Select optimisation level for GEANT geometry searches (0,1,2)
    geant3->SetERAN(5.e-7);

    /* Geant3 Phyics Settings
     * See http://hep.fi.infn.it/geant.pdf Page 188ff for more details
     *
     * These setting are (should be) equivalent to the setting in this file combined with the
     * gMC->SetProcess("name", id) calls previously used in SetCuts.C
     */

    // Cerenkov photon generation
    geant3->SetCKOV(1); // =1 (??) Cerenkov

    // Rayleigh effect.
    geant3->SetRAYL(1); // =1 Rayleigh effect.
    // =0 (Default) No Rayleigh effect.

    // Pair production.
    geant3->SetPAIR(1); // =1 (Default) Pair production with generation of e-/e+

    // Compton scattering.
    geant3->SetCOMP(1); // =1 (Default) Compton scattering with generation of e-

    // Photoelectric effect.
    geant3->SetPHOT(1); // =1 (Default) Photo-electric effect with generation of the electron

    // Nuclear fission induced by a photon.
    geant3->SetPFIS(0); // =0 (Default) No photo-fission
    // TODO: Evaluate usage of "=1 Photo-fission with generation of secondaries"

    // delta-ray production.
    geant3->SetDRAY(1); // =1 (Default) delty-rays production with generation of e-
    // TODO: Evaluate "=2 delta-rays production without generation of e-" instead of cutting e-

    // Positron annihilation.
    geant3->SetANNI(1); // =1 (Default) Positron annihilation with generation of photons.

    // bremsstrahlung
    geant3->SetBREM(1); // =1 (Default) bremsstrahlung with generation of gamma

    // Hadronic interactions.
    geant3->SetHADR(5); // Use a user code hadronic package: =5 GCALOR
    // =1 (Default) Hadronic interactions with generation of secondaries. (Do not use this)

    // Muon-nucleus interactions.
    geant3->SetMUNU(1); // =1 (Default) Muon-nucleus interactions with generation of secondaries.

    // Decay in flight.
    geant3->SetDCAY(1); // =1 (Default) Decay in flight with generation of secondaries

    // Continuous energy loss.
    geant3->SetLOSS(1); // =1 Continuous energy loss with generation of δ-rays above DCUTE (common /GCUTS/) and
                        // restricted Landau fluctuations below DCUTE. (=3 Same as 1, kept for backward compatibility)

    // Multiple scattering.
    geant3->SetMULS(1); // =1 (Default) Multiple scattering according to Moliere theory

    // Collision sampling method to simulate energy loss in thin material
    geant3->SetSTRA(1); // =1 Collision sampling activated.
    // =0 (Default) Collision sampling switched off.

    // Not Set: SYNC =0 (Default) The synchrotron radiation is not simulated.
}
