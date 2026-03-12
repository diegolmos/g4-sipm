#include "Materials.hh"
#include <G4NistManager.hh>
#include <G4SystemOfUnits.hh>
#define LYANTHRACENE 17400
namespace Materials {

    G4Material* CreateBC412(){
        G4NistManager* nist = G4NistManager::Instance();
        G4Material* mat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
        
        // Curva espectral de BC-412
        const G4int nEntries = 30;
        G4double wavelength[nEntries] = {
            400, 404, 408, 412, 417, 421, 425, 429,
            433, 437, 441, 446, 450, 454, 458, 462,
            466, 470, 475, 479, 483, 487, 491, 495,
            499, 503, 508, 512, 516, 520
        };
        
        G4double relLightOutput[nEntries] = {
            12,  20,  32,  45,  60,  75,  88,  97,
            100,  98,  95,  92,  88,  83,  78,  73,
            67,  62,  56,  51,  46,  42,  38,  33,
            29,  25,  21,  18,  14,  11
        };
        
        G4double photonEnergy[nEntries];
        G4double fastComponent[nEntries];
        
        const G4double h_Planck = 4.135667696e-15 * eV * s;
        const G4double c_light = 2.99792458e8 * m/s;
        const G4double hc = h_Planck * c_light;
        
        for (int i = 0; i < nEntries; ++i) {
            photonEnergy[i] = hc / (wavelength[i] * nm);     // eV
            fastComponent[i] = relLightOutput[i] / 100.0;      // normalizado
        }
        std::reverse(photonEnergy, photonEnergy + nEntries);
        std::reverse(fastComponent, fastComponent + nEntries);
        
        std::vector<G4double> energy     = {2.034*eV, 3.*eV, 4.136*eV};
        std::vector<G4double> rindex     = {1.58, 1.58, 1.58};
        std::vector<G4double> absorption = {344.8*cm, 850.*cm, 1450.0*cm};
        
        auto MPT = new G4MaterialPropertiesTable();
        MPT->AddProperty("FASTCOMPONENT", photonEnergy, fastComponent, nEntries, true);
        MPT->AddConstProperty("SCINTILLATIONYIELD", 0.6*LYANTHRACENE / MeV);
        MPT->AddConstProperty("RESOLUTIONSCALE", 1.0);
        MPT->AddConstProperty("FASTTIMECONSTANT", 3.3 * ns, true);
        MPT->AddConstProperty("YIELDRATIO", 1.0, true);
        MPT->AddProperty("RINDEX", energy,rindex,3, true);
        MPT->AddProperty("ABSLENGTH", energy,absorption,3, true);
        
        mat->SetMaterialPropertiesTable(MPT);
        return mat;
    }

    G4Material* CreateBC408(){
        G4NistManager* nist = G4NistManager::Instance();

        G4Material* mat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
        
        // Curva espectral de BC-408
        const G4int nEntries = 30;
        G4double wavelength[nEntries] = {
            360, 365, 370, 375, 380, 385, 390, 395, 400, 405,
            410, 415, 420, 425, 430, 435, 440, 445, 450,
            460, 465, 470, 475, 480, 485, 490, 495, 510, 520
        }; // en nm

        G4double relLightOutput[nEntries] = {
            2,   3,   4,   5,  6,  7,  10,  15,  22,  38,
            45,  65,  83,  94, 100,  97,  92,  84,  75,  64,
            52,  41,  32,  25,  19,  14,  10,   7,   3,   1
        }; // relativa (0–100)

        G4double photonEnergy[nEntries];
        G4double fastComponent[nEntries];
        
        const G4double h_Planck = 4.135667696e-15 * eV * s;
        const G4double c_light = 2.99792458e8 * m/s;
        const G4double hc = h_Planck * c_light;
        
        for (int i = 0; i < nEntries; ++i) {
            photonEnergy[i] = hc / (wavelength[i] * nm);     // eV
            fastComponent[i] = relLightOutput[i] / 100.0;      // normalizado
        }
        std::reverse(photonEnergy, photonEnergy + nEntries);
        std::reverse(fastComponent, fastComponent + nEntries);
        
        auto MPT = new G4MaterialPropertiesTable();
        MPT->AddProperty("FASTCOMPONENT", photonEnergy, fastComponent, nEntries, true);
        MPT->AddConstProperty("SCINTILLATIONYIELD", 0.64*LYANTHRACENE / MeV);
        MPT->AddConstProperty("RESOLUTIONSCALE", 1.0);
        MPT->AddConstProperty("FASTTIMECONSTANT", 2.1 * ns, true);
        MPT->AddConstProperty("YIELDRATIO", 1.0, true);
        MPT->AddConstProperty("RINDEX", 1.58, true);
        MPT->AddConstProperty("ABSLENGTH", 380. * cm, true);
        
        mat->SetMaterialPropertiesTable(MPT);
        return mat;
    }

    G4OpticalSurface* CreateSiPMSurface(){
        G4NistManager* nist = G4NistManager::Instance();

        const G4double h_Planck = 4.135667696e-15 * eV * s;
        const G4double c_light = 2.99792458e8 * m/s;
        const G4double hc = h_Planck * c_light;
        const G4double fillFactor = 0.74; //included in pde
        
        const G4int nEntries = 33;
        G4double wavelength_nm[nEntries] = {
            300, 320, 340, 360, 380, 400, 420, 440, 460, 480, 500,
            520, 540, 560, 580, 600, 620, 640, 660, 680, 700,
            720, 740, 760, 780, 800, 820, 840, 860, 880, 900,
            920, 940
        };
        G4double pde_raw[nEntries] = {
            0.00, 0.05, 0.12, 0.18, 0.26, 0.33, 0.38, 0.40, 0.405, 0.403, 0.39,
            0.37, 0.34, 0.31, 0.28, 0.24, 0.21, 0.18, 0.15, 0.12, 0.10,
            0.08, 0.06, 0.05, 0.04, 0.03, 0.025, 0.02, 0.015, 0.01, 0.008,
            0.004, 0.001
        };
        
        G4double photonEnergy[nEntries];
        G4double detectionEfficiency[nEntries];
        
        for (int i = 0; i < nEntries; ++i) {
            photonEnergy[i] = (hc / (wavelength_nm[i] * nm)); // eV
            detectionEfficiency[i] = pde_raw[i];
        }
        
        std::reverse(photonEnergy, photonEnergy + nEntries);
        std::reverse(detectionEfficiency, detectionEfficiency + nEntries);
        
        auto MPT = new G4MaterialPropertiesTable();
        MPT->AddProperty("DETECTIONEFFICIENCY", photonEnergy, detectionEfficiency, nEntries,true);
  
        //std::vector<G4double> pE = { 2.0*eV, 3.5*eV };
        //std::vector<G4double> ref = { 0.1, 0.1 };
        //MPT->AddProperty("REFLECTIVITY", pE, ref);

        auto sipmSurface = new G4OpticalSurface("SiPMSurface");
        sipmSurface->SetType(dielectric_dielectric);
        sipmSurface->SetModel(unified);
        sipmSurface->SetFinish(polished);
        //sipmSurface->SetPolish(0.8); // entre 0.0 (muy rugosa) y 1.0 (perfectamente pulida)
        sipmSurface->SetMaterialPropertiesTable(MPT);
        
        
        return sipmSurface;
    }
}
