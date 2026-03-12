#ifndef MATERIALS_HH
#define MATERIALS_HH

#include <G4Material.hh>
#include <G4MaterialPropertiesTable.hh>
#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4VisAttributes.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalBorderSurface.hh>

namespace Materials {
    G4Material* CreateBC408();
    G4Material* CreateBC412();
    G4OpticalSurface* CreateSiPMSurface();
}
#endif

