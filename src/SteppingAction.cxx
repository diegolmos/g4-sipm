#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4OpticalPhoton.hh"
#include "G4SystemOfUnits.hh"
#include <iostream>
#include "G4VProcess.hh"


SteppingAction::SteppingAction() {G4cout << ">>>>>>>>>>>>>>>Registered SteppingAction!" << G4endl; }
SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    auto track = step->GetTrack();
    auto particle = track->GetParticleDefinition();
    auto process = step->GetPostStepPoint()->GetProcessDefinedStep();
 
    if (step->GetTrack()->GetDefinition() == G4OpticalPhoton::Definition()) {
        auto pre = step->GetPreStepPoint()->GetPhysicalVolume();
        auto post = step->GetPostStepPoint()->GetPhysicalVolume();

        if (post && pre && pre->GetName() != post->GetName()) {
            auto proc = step->GetPostStepPoint()->GetProcessDefinedStep();
            if (proc) {
                G4cout << ">>> OpticalPhoton crossed from " << pre->GetName()
                       << " to " << post->GetName() << ", process: "
                       << proc->GetProcessName() << G4endl;
            }
        }
    }

    G4cout << ">>>>>>> Step by: " << particle->GetParticleName() << G4endl;
    
    G4double edep = step->GetTotalEnergyDeposit();
    if (edep > 0)
      G4cout << ">>>>>>>>>>>>>> Energy deposit: " << edep / keV << " keV" << G4endl;

    if (particle == G4OpticalPhoton::Definition()) {
        G4ThreeVector pos = track->GetPosition();
        G4double time = track->GetGlobalTime();
        G4cout << ">>>>>>>>>>>>>>>>>>>>>>>[Photon] Step at position " << pos/cm
               << " cm, time: " << time/ns << " ns" << G4endl;
    }
}

