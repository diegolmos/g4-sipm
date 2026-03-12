#ifndef RunAction_h
#define RunAction_h 1

#include <G4UserRunAction.hh>
#include <G4String.hh>
#include <TFile.h>
#include <TTree.h>

class RunAction : public G4UserRunAction
{
public:
    RunAction(int nsipms =0);
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);
    void AddBranch(const G4String& name, double* address, const G4String& leaflist);
    int GetNSiPMs() const { return fNSiPMs; }
    TTree* GetTree() const { return fTree; }
    TFile* GetFile() const { return fFile; }

private:
    int fNSiPMs;
    TFile* fFile;
    TTree* fTree;
};

#endif
