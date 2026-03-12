#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

ActionInitialization::ActionInitialization(DetectorConstruction *det) {fDetector = det;}
ActionInitialization::~ActionInitialization() {}

void ActionInitialization::Build() const
{
    auto runAction = new RunAction(fDetector->GetNSiPMs());
    SetUserAction(runAction);

    auto eventAction = new EventAction(runAction,fDetector->GetNSiPMs());
    SetUserAction(eventAction);

    SetUserAction(new PrimaryGeneratorAction());
    //SetUserAction(new SteppingAction());
    
}
