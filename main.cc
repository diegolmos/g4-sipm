#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

int main(int argc, char** argv)
{
  // Fuerza modo SERIAL (este código escribe ROOT y no está pensado para MT)
  auto* runManager =
      G4RunManagerFactory::CreateRunManager(G4RunManagerType::Serial);

  auto* det = new DetectorConstruction(4);
  runManager->SetUserInitialization(det);

  // Física estándar + óptica (necesaria para scintillation / optical photons)
  auto* phys = new FTFP_BERT;
  phys->RegisterPhysics(new G4OpticalPhysics());
  runManager->SetUserInitialization(phys);

  // Actions (usa el detector para saber cuántos SiPMs hay)
  runManager->SetUserInitialization(new ActionInitialization(det));

  // Vis
  auto* visManager = new G4VisExecutive();
  visManager->Initialize();

  auto* UImanager = G4UImanager::GetUIpointer();

  if (argc == 1) {
    // Interactivo: inicializa y ejecuta el macro de visualización
    auto* ui = new G4UIExecutive(argc, argv);
    UImanager->ApplyCommand("/run/initialize");
    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    delete ui;
  } else {
    // Batch: ./xyplane_sim run.mac
    G4String cmd = "/control/execute ";
    UImanager->ApplyCommand(cmd + G4String(argv[1]));
  }

  delete visManager;
  delete runManager;
  return 0;
}
