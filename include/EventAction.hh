#ifndef EventAction_h
#define EventAction_h 1

#include <G4UserEventAction.hh>
#include "RunAction.hh"

class EventAction : public G4UserEventAction
{
public:
    EventAction(RunAction* runAction = nullptr, int nsipms = 0);
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

    void AddSiPMHit(int sipmIndex, double light) const;
    int GetNSiPMs() const { return fNSiPMs; }


private:
    RunAction* fRunAction;
    double *fLightCollected;
    int fNSiPMs;
};

#endif
