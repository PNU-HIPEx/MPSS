#include "TTrackingAction.hpp"

#include "TAnalysisManager.hpp"

void TTrackingAction::PreUserTrackingAction(const G4Track* track) {
	TAnalysisManager* analysisManager = TAnalysisManager::Instance();
	analysisManager->doPreTracking(track);
}

void TTrackingAction::PostUserTrackingAction(const G4Track* track) {
	TAnalysisManager* analysisManager = TAnalysisManager::Instance();
	analysisManager->doPostTracking(track);
}