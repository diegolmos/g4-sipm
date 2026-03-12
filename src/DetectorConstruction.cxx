#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4RotationMatrix.hh>
#include <G4VisAttributes.hh>
#include <G4SDManager.hh>
#include <G4OpticalSurface.hh>
#include <G4MaterialPropertiesTable.hh>
#include <G4LogicalBorderSurface.hh>
#include "TROOT.h"
#include "TString.h"
#include "Materials.hh"


DetectorConstruction::DetectorConstruction(int nsipms) {fNSiPMs = nsipms;}
DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    G4NistManager* nist = G4NistManager::Instance();

    // Materiales
    G4Material* worldMat = nist->FindOrBuildMaterial("G4_AIR");
    auto BC412 = Materials::CreateBC412();
    G4Material* sipmMat = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    auto mpt = new G4MaterialPropertiesTable();
    std::vector<G4double> energy     = {2.034*eV, 3.*eV, 4.136*eV};
    std::vector<G4double> rindex     = {1.58, 1.58, 1.58};
    //std::vector<G4double> absorption = {344.8*cm, 850.*cm, 1450.0*cm};

    mpt->AddProperty("RINDEX",energy, rindex, 3,  true); // BC412
    sipmMat->SetMaterialPropertiesTable(mpt);
    // Tamaños
    G4double scintSizeXY = 6.0 * cm;
    G4double scintSizeZ  = 15.0 * mm;
    G4double sipmSize    = 6.0 * mm;
    G4double sipmSizeZ   = 1 * mm;
    G4double worldSize   = 10 * cm;
    const G4int nSiPMs   = 4;

    // Volumen del mundo
    G4Box* worldSolid = new G4Box("World", worldSize, worldSize, worldSize);
    fWorldLogical = new G4LogicalVolume(worldSolid, worldMat, "World");
    fWorldLogical->SetVisAttributes(G4VisAttributes::GetInvisible());
    G4VPhysicalVolume* worldPhys = new G4PVPlacement(
        nullptr, G4ThreeVector(), fWorldLogical, "World", nullptr, false, 0, true);

    // Plástico centellador
    G4Box* scintSolid = new G4Box("Scintillator", scintSizeXY/2, scintSizeXY/2, scintSizeZ/2);
    fScintLogical = new G4LogicalVolume(scintSolid, BC412, "Scintillator");
    fScintPhysical = new G4PVPlacement(nullptr, G4ThreeVector(), fScintLogical, "Scintillator", fWorldLogical, false, 0, true);

    // Visualización del plástico
    auto visScint = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0,0.2)); // Azul translucido
    visScint->SetForceSolid(true);
    fScintLogical->SetVisAttributes(visScint);
    
    auto opticalSurface = new G4OpticalSurface("ScintSurface");
    opticalSurface->SetType(dielectric_dielectric);
    opticalSurface->SetModel(unified);
    opticalSurface->SetFinish(groundfrontpainted); // o polished, etc.
    opticalSurface->SetSigmaAlpha(0.1); // rugosidad (si es ground)
    
    auto surface = new G4LogicalBorderSurface(
        "ScintSurface",
        fScintPhysical,  // volumen 1
        worldPhys,         // volumen 2 o cualquier otro volumen adyacente
        opticalSurface);       // puntero a G4OpticalSurface

    G4MaterialPropertiesTable* surfaceMPT = new G4MaterialPropertiesTable();

    std::vector<G4double> photonEnergy = { 2.0*eV, 3.5*eV }; // ajusta el rango
    std::vector<G4double> reflectivity = { 0.95, 0.95 };       // 100% reflectividad

    surfaceMPT->AddProperty("REFLECTIVITY", photonEnergy, reflectivity);
    opticalSurface->SetMaterialPropertiesTable(surfaceMPT);

    
   
    // --- SiPMs ---

    G4Box* sipmSolid = new G4Box("SiPM", sipmSize/2, sipmSize/2, 0.5*mm);
    fSiPMLogical = new G4LogicalVolume(sipmSolid, sipmMat, "SiPM");

    auto visSiPM = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0,0.2)); // Rojo
    visSiPM->SetForceSolid(true);
    fSiPMLogical->SetVisAttributes(visSiPM);

    G4ThreeVector sipmPos[nSiPMs] = {
        G4ThreeVector(0,  (scintSizeXY + sipmSizeZ)/2, (scintSizeZ - sipmSize)/2),
        G4ThreeVector(0, -(scintSizeXY + sipmSizeZ)/2, (scintSizeZ - sipmSize)/2),
        G4ThreeVector( (scintSizeXY + sipmSizeZ)/2, 0, (scintSizeZ - sipmSize)/2),
        G4ThreeVector(-(scintSizeXY + sipmSizeZ)/2, 0, (scintSizeZ - sipmSize)/2)
    };

    G4RotationMatrix* rotY90 = new G4RotationMatrix();
    G4RotationMatrix* rotX90 = new G4RotationMatrix();

    rotY90->rotateX(90 * deg);
    rotX90->rotateY(90 * deg);


    auto sipmSurface = Materials::CreateSiPMSurface();
    for (int i = 0; i < nSiPMs; ++i) {
        G4RotationMatrix* rot = (i < 2) ? rotY90 : rotX90;

        G4VPhysicalVolume* sipmPhys = new G4PVPlacement(
            rot, sipmPos[i], fSiPMLogical, "SiPM", fWorldLogical, false, i, true);

        // Crear frontera óptica entre el plástico y este SiPM
        fSciSiPMB[i] = new G4LogicalBorderSurface(
            Form("SiPMSurface_%d", i),
            fScintPhysical, sipmPhys,
            sipmSurface
        );
        
        G4LogicalBorderSurface* checkSurface =
        G4LogicalBorderSurface::GetSurface(fScintPhysical, sipmPhys);

        if (checkSurface) {
            G4cout << ">>>> optical surface between "
                   << fScintPhysical->GetName() << " and " << sipmPhys->GetName()
                   << " found! -- sipm "<<i << G4endl;
        } else {
            G4cout << ">>>> WARNING:: NO optical surface between"
                   << fScintPhysical->GetName() << " and " << sipmPhys->GetName()
                   <<" -- sipm "<<i<<G4endl;
        }

    }

    return worldPhys;
}

void DetectorConstruction::ConstructSDandField()
{
    auto sdManager = G4SDManager::GetSDMpointer();
    auto sipmSD = new SensitiveDetector("SiPMSD",fSciSiPMB);
    sdManager->AddNewDetector(sipmSD);
    SetSensitiveDetector("SiPM", sipmSD);
}
