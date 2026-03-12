#include "RunAction.hh"
#include "G4OpticalParameters.hh"

RunAction::RunAction(int nsipms):fNSiPMs(nsipms)
{
    fFile = new TFile("output.root", "RECREATE");
    fTree = new TTree("Hits", "SiPM Hits");
}

RunAction::~RunAction()
{
    if (fFile) delete fFile;
}

void RunAction::BeginOfRunAction(const G4Run*)
{
    G4OpticalParameters::Instance()->SetScintTrackSecondariesFirst(true);
}

void RunAction::EndOfRunAction(const G4Run*)
{
    if (fTree) fTree->Write();
    if (fFile) fFile->Close();
}

void RunAction::AddBranch(const G4String& name, double* address, const G4String& leaflist)
{
    if (fTree)  {
        fTree->Branch(name, address, leaflist.data());
        std::cout<<">>>>>>>>>>>>>>>>>>>>branch "<<name<<" added\n";
    }
}
