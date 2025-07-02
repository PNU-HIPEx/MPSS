#include "TSteppingAction.hpp"

#include "TAnalysisManager.hpp"

TSteppingAction::TSteppingAction(TEventAction* eventAction) : G4UserSteppingAction(), fEventAction(eventAction) { }

void TSteppingAction::UserSteppingAction(const G4Step* step) {
	TAnalysisManager* analysisManager = TAnalysisManager::Instance();
	analysisManager->doStepPhase(step);
}