#include "EventAction.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4String.hh"

EventAction::EventAction(RunAction* runAction, int nsipms)
  : fRunAction(runAction), fNSiPMs(nsipms)
{
    fLightCollected = new double[fNSiPMs];
    if(!runAction){
        G4cerr<<"runAction is nullprt\n Aborting\n";
        exit(1);
    }
   // for (int i = 0; i < fNSiPMs; ++i) {
        fRunAction->AddBranch("sipm", fLightCollected, (TString("sipm[") + fNSiPMs +"]/D").Data());
   // }
}

EventAction::~EventAction() {delete[] fLightCollected;}

void EventAction::BeginOfEventAction(const G4Event*)
{
    for (int i = 0; i < fNSiPMs; ++i) fLightCollected[i] = 0.;
}

void EventAction::EndOfEventAction(const G4Event*)
{
    fRunAction->GetTree()->Fill();
}

void EventAction::AddSiPMHit(int sipmIndex, double light) const
{
    if (sipmIndex >= 0 && sipmIndex < fNSiPMs) {
        fLightCollected[sipmIndex] += light;
    }
}
