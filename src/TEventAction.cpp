#include "TEventAction.hpp"

#include "TRunAction.hpp"
#include "TAnalysisManager.hpp"
#include "G4AnalysisManager.hh"
#include "G4Event.hh"

TEventAction::TEventAction(TRunAction* runAction) : G4UserEventAction(), fRunAction(runAction) { }

void TEventAction::BeginOfEventAction(const G4Event* event) {
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->FillNtupleIColumn(0, event->GetEventID());
}

void TEventAction::EndOfEventAction(const G4Event* event) {
}