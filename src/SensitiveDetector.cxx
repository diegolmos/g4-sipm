#include "SensitiveDetector.hh"
#include "EventAction.hh"
#include <G4StepPoint.hh>
#include <G4Track.hh>
#include <G4OpticalPhoton.hh>
#include "G4LogicalBorderSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4SurfaceProperty.hh"
#include "G4RandomTools.hh"
#include <G4SystemOfUnits.hh>
#include "G4RunManager.hh"


SensitiveDetector::SensitiveDetector(const G4String& name, std::map<int,G4LogicalBorderSurface*> SciSiPMB, int index, EventAction* eventAction)
  : G4VSensitiveDetector(name), fSciSiPMB(SciSiPMB), fIndex(index)
{
    fEventAction = dynamic_cast<const EventAction*>(G4RunManager::GetRunManager()->GetUserEventAction());
    for (const auto& pair : fSciSiPMB)
        G4cout << "key/value -- " << pair.first << "/"<<pair.second<<G4endl;
    
    
    
    const G4MaterialPropertyVector* vec = dynamic_cast<G4OpticalSurface*> (fSciSiPMB[0]->GetSurfaceProperty())->GetMaterialPropertiesTable()->GetProperty("DETECTIONEFFICIENCY");

    if (vec) {
        G4cout << ">>>>> DETECTIONEFFICIENCY:" << G4endl;
        for (size_t i = 0; i < vec->GetVectorLength(); ++i) {
            G4double energy = vec->Energy(i); // en eV
            G4double value = (*vec)[i];       // eficiencia (sin unidades)
            G4cout << "  E = " << energy / eV << " eV -> " << value << G4endl;
        }
    } else {
        G4cout << "No DETECTIONEFFICIENCY found in the MPT!" << G4endl;
    }
}

SensitiveDetector::~SensitiveDetector() {}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    // Solo contar fotones ópticos
    auto particle = step->GetTrack()->GetDefinition();
    if (particle != G4OpticalPhoton::OpticalPhotonDefinition()) return false;
    
    G4StepPoint* preStepPoint = step->GetPreStepPoint();
    if (preStepPoint->GetStepStatus() != fGeomBoundary){
        G4cout<<">>>> photon in sipm not comming from the boundary\n";

        step->GetTrack()->SetTrackStatus(fStopAndKill); // not counting photons not comming from the surface
        return 0;
    }
   
    const G4VProcess* process = preStepPoint->GetProcessDefinedStep();

    G4TouchableHandle touchable = preStepPoint->GetTouchableHandle();
    G4int sipmIndex = touchable->GetCopyNumber();  // útil si tienes varios SiPM

    
    G4cout<<">>>> evaluating photon in sipm "<<sipmIndex<<"\n";

    G4double photonEnergy = preStepPoint->GetKineticEnergy();
    G4MaterialPropertyVector* detectionEfficiency = nullptr;

    // getting optical surface, not working G4Step is created en SD.
    
    G4cout<<">>>> tryting to get surface between  "<<preStepPoint->GetPhysicalVolume()->GetName()<<" and "
    << step->GetPostStepPoint()->GetPhysicalVolume()->GetName()<<"\n";
    G4LogicalBorderSurface* borderSurface =
        G4LogicalBorderSurface::GetSurface(preStepPoint->GetPhysicalVolume(), step->GetPostStepPoint()->GetPhysicalVolume());

    borderSurface = fSciSiPMB[sipmIndex];
    G4cout<<">>>>border surface used -- "<<borderSurface->GetName()<<G4endl;
    
    G4OpticalSurface* opticalSurface = dynamic_cast<G4OpticalSurface*> (borderSurface->GetSurfaceProperty());
    
    if (opticalSurface) {
        G4cout<<">>>> optical surface: "<<opticalSurface->GetName()<<"\n";

        G4MaterialPropertiesTable* MPT = opticalSurface->GetMaterialPropertiesTable();
        if (MPT) {
            detectionEfficiency = MPT->GetProperty("DETECTIONEFFICIENCY");
        }
    }

    G4cout<<">>>> detection efficiency: "<<detectionEfficiency<<"\n";

    if (detectionEfficiency) {
        G4double efficiency = detectionEfficiency->Value(photonEnergy);
        G4cout<<">>>> E/efficiency -- "<<photonEnergy/eV<<" eV/"<<efficiency<<"\n";

        if (G4UniformRand() > efficiency) {
            
            step->GetTrack()->SetTrackStatus(fStopAndKill); // si no se detecta, se elimina
            return false; //
        }
    }
    else {
        step->GetTrack()->SetTrackStatus(fStopAndKill); // si no se detecta, se elimina
        return false; //
    }
    
    G4cout<<">>>>>>>>>> photon catched on sipm:"<<sipmIndex<<"\n";
    fEventAction->AddSiPMHit(sipmIndex, 1.0);  // Cuenta 1 unidad de luz por cada fotón
    step->GetTrack()->SetTrackStatus(fStopAndKill); // Evita doble conteo

    return true;
}
