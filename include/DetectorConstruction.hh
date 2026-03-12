#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1
#include <map>

#include <G4VUserDetectorConstruction.hh>
#include <G4ThreeVector.hh>
#include "G4LogicalBorderSurface.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    //DetectorConstruction();
    DetectorConstruction(int nsipms = 4);
    virtual ~DetectorConstruction();
    virtual G4VPhysicalVolume* Construct() override;
    virtual void ConstructSDandField() override;
    virtual int GetNSiPMs() const { return fNSiPMs; }

private:
    // Lógicos y físicos
    G4LogicalVolume* fWorldLogical;
    G4LogicalVolume* fScintLogical;
    G4LogicalVolume* fSiPMLogical;
    G4VPhysicalVolume* fScintPhysical;

    // Parámetros geométricos
    G4double scintSizeXY;
    G4double scintSizeZ;
    G4double sipmSize;
    G4int fNSiPMs;
    std::map<int,G4LogicalBorderSurface*> fSciSiPMB;


    // Fill factor del SiPM
    G4double fillFactor;
};

#endif
