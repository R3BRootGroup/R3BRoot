//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: G4LowEnergyQEDBuilder.cc,v 1.2 2005/08/23 17:31:13 vnivanch Exp $
// GEANT4 tag $Name: geant4-08-00-patch-01 $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4LowEnergyQEDBuilder
//
// Author:      V.Ivanchenko 03.05.2004
//
// Modified:
//
//----------------------------------------------------------------------------
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4Version.hh"
#include "G4LowEnergyQEDBuilder.h"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

#if G4VERSION_NUMBER > 922
#include "G4eMultipleScattering.hh"
#else
#include "G4MultipleScattering.hh"
#endif

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

// ----- G4 version lower than 9.5 ------ 
#if G4VERSION_NUMBER < 950
#include "G4LowEnergyCompton.hh"
#include "G4LowEnergyGammaConversion.hh"
#include "G4LowEnergyPhotoElectric.hh"
#include "G4LowEnergyRayleigh.hh"

#include "G4LowEnergyIonisation.hh"
#include "G4LowEnergyBremsstrahlung.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"


// ----- G4 version 9.5 ------ 
#else

#include "G4PhysicsListHelper.hh"
// gamma
#include "G4PhotoElectricEffect.hh"
#include "G4LivermorePhotoElectricModel.hh"

#include "G4ComptonScattering.hh"
#include "G4LivermoreComptonModel.hh"

#include "G4GammaConversion.hh"
#include "G4LivermoreGammaConversionModel.hh"

#include "G4RayleighScattering.hh" 
#include "G4LivermoreRayleighModel.hh"

// e-
#include "G4eMultipleScattering.hh"
#include "G4UniversalFluctuation.hh"

#include "G4eIonisation.hh"
#include "G4LivermoreIonisationModel.hh"

#include "G4eBremsstrahlung.hh"
#include "G4LivermoreBremsstrahlungModel.hh"

// e+
#include "G4eplusAnnihilation.hh"
#include "G4PenelopeAnnihilationModel.hh"

#endif


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4LowEnergyQEDBuilder::G4LowEnergyQEDBuilder(const G4String& name)
   :  G4VPhysicsConstructor(name)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4LowEnergyQEDBuilder::~G4LowEnergyQEDBuilder()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4LowEnergyQEDBuilder::ConstructParticle()
{
  G4Gamma::Gamma();
  G4Electron::Electron();
  G4Positron::Positron();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4LowEnergyQEDBuilder::ConstructProcess()
{
 
// ----- G4 version lower than 9.5 ------ 
#if G4VERSION_NUMBER < 950

  // Add standard EM Processes for gamma
  G4ParticleDefinition* particle = G4Gamma::Gamma();
  G4ProcessManager* pmanager = particle->GetProcessManager();

  G4LowEnergyPhotoElectric* pe = new G4LowEnergyPhotoElectric();
  pe->SetAngularGenerator("standard");

  pmanager->AddDiscreteProcess( pe );
  pmanager->AddDiscreteProcess( new G4LowEnergyCompton() );
  pmanager->AddDiscreteProcess( new G4LowEnergyGammaConversion() );
  pmanager->AddDiscreteProcess( new G4LowEnergyRayleigh() );

  // Add standard EM Processes for e-
  particle = G4Electron::Electron();
  pmanager = particle->GetProcessManager();

 
#if G4VERSION_NUMBER > 922
        pmanager->AddProcess(new G4eMultipleScattering,-1,1,1);
#else
        pmanager->AddProcess(new G4MultipleScattering,-1,1,1);
#endif

  pmanager->AddProcess(new G4LowEnergyIonisation,      -1, 2,2);
  pmanager->AddProcess(new G4LowEnergyBremsstrahlung,  -1,-1,3);

  // Add standard EM Processes for e+
  particle = G4Positron::Positron();
  pmanager = particle->GetProcessManager();

 #if G4VERSION_NUMBER > 922
        pmanager->AddProcess(new G4eMultipleScattering,-1,1,1);
#else
        pmanager->AddProcess(new G4MultipleScattering,-1,1,1);
#endif
 
  pmanager->AddProcess(new G4eIonisation,        -1, 2,2);
  pmanager->AddProcess(new G4eBremsstrahlung,    -1,-1,3);
  pmanager->AddProcess(new G4eplusAnnihilation,   0,-1,4);


// ----- G4 version 9.5 ------ 
#else

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

  // Add standard EM Processes for gamma
  G4ParticleDefinition* particle = G4Gamma::Gamma();

  G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
  thePhotoElectricEffect->SetEmModel(new G4LivermorePhotoElectricModel());
  ph->RegisterProcess(thePhotoElectricEffect, particle);

  G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
  theComptonScattering->SetEmModel(new G4LivermoreComptonModel());
  ph->RegisterProcess(theComptonScattering, particle);

  G4GammaConversion* theGammaConversion = new G4GammaConversion();
  theGammaConversion->SetEmModel(new G4LivermoreGammaConversionModel());
  ph->RegisterProcess(theGammaConversion, particle);

  G4RayleighScattering* theRayleigh = new G4RayleighScattering();
  theRayleigh->SetEmModel(new G4LivermoreRayleighModel());
  ph->RegisterProcess(theRayleigh, particle);
    
  // Add standard EM Processes for e-
  particle = G4Electron::Electron();

  G4eMultipleScattering* msc = new G4eMultipleScattering();
  ph->RegisterProcess(msc, particle);
     
  // Ionisation
  G4eIonisation* eIoni = new G4eIonisation();
  eIoni->SetEmModel(new G4LivermoreIonisationModel());
  eIoni->SetFluctModel(new G4UniversalFluctuation() );
  ph->RegisterProcess(eIoni, particle);
      
  // Bremsstrahlung
  G4eBremsstrahlung* eBrem = new G4eBremsstrahlung();
  eBrem->SetEmModel(new G4LivermoreBremsstrahlungModel());
  ph->RegisterProcess(eBrem, particle);

  // Add standard EM Processes for e+
  particle = G4Positron::Positron();

  ph->RegisterProcess(msc, particle);

  //Annihilation
  G4eplusAnnihilation* eAnni = new G4eplusAnnihilation();
  eAnni->SetEmModel(new G4PenelopeAnnihilationModel());
  ph->RegisterProcess(eAnni, particle);

#endif

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

