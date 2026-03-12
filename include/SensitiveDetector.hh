#ifndef SensitiveDetector_h
#define SensitiveDetector_h 1
#include <map>
#include <G4VSensitiveDetector.hh>
#include <G4Step.hh>
#include "G4LogicalBorderSurface.hh"

class EventAction;

class SensitiveDetector : public G4VSensitiveDetector
{
public:
    SensitiveDetector(const G4String& name, std::map<int,G4LogicalBorderSurface*> SciSiPMB = {}, int index=0, EventAction* eventAction=nullptr);
    virtual ~SensitiveDetector();

    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;

private:
    int fIndex; // Identificador del SiPM (0-3)
    const EventAction* fEventAction;
    std::map<int,G4LogicalBorderSurface*> fSciSiPMB;
};

#endif
