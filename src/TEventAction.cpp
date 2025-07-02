#include "TEventAction.hpp"

#include "TRunAction.hpp"
#include "TAnalysisManager.hpp"

TEventAction::TEventAction(TRunAction* runAction) : G4UserEventAction(), fRunAction(runAction) { }

void TEventAction::BeginOfEventAction(const G4Event* event) {
	TAnalysisManager* analysisManager = TAnalysisManager::Instance();
	analysisManager->doBeginOfEvent(event);
}

void TEventAction::EndOfEventAction(const G4Event* event) {
	TAnalysisManager* analysisManager = TAnalysisManager::Instance();
	analysisManager->doEndOfEvent(event);
}