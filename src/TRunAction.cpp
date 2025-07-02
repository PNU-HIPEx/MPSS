#include "TRunAction.hpp"

#include "TAnalysisManager.hpp"

void TRunAction::BeginOfRunAction(const G4Run* run) {
	TAnalysisManager* analysisManager = TAnalysisManager::Instance();
	analysisManager->doBeginOfRun(run);
}

void TRunAction::EndOfRunAction(const G4Run* run) {
	TAnalysisManager* analysisManager = TAnalysisManager::Instance();
	analysisManager->doEndOfRun(run);
}

