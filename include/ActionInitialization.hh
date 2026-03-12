#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include <G4VUserActionInitialization.hh>
#include <DetectorConstruction.hh>

class ActionInitialization : public G4VUserActionInitialization
{
public:
    ActionInitialization(DetectorConstruction *det = nullptr);
    virtual ~ActionInitialization();
    
    virtual void Build() const override;

private:
    DetectorConstruction *fDetector;
};

#endif


