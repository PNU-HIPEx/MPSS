#include "TTrackingAction.hpp"
#include "G4AnalysisManager.hh"
#include "G4Track.hh"

void TTrackingAction::PreUserTrackingAction(const G4Track* track) {
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->FillNtupleIColumn(1, track->GetTrackID());
	analysisManager->FillNtupleIColumn(2, track->GetParentID());
	analysisManager->FillNtupleIColumn(3, track->GetParticleDefinition()->GetPDGEncoding());
	analysisManager->FillNtupleDColumn(4, track->GetPosition().x());
	analysisManager->FillNtupleDColumn(5, track->GetPosition().y());
	analysisManager->FillNtupleDColumn(6, track->GetPosition().z());
	analysisManager->FillNtupleDColumn(7, track->GetMomentum().x());
	analysisManager->FillNtupleDColumn(8, track->GetMomentum().y());
	analysisManager->FillNtupleDColumn(9, track->GetMomentum().z());

	if ( track->GetParentID() == 0 ) {
		analysisManager->FillH2(0, track->GetPosition().x(), track->GetPosition().z());
		analysisManager->FillH1(0, track->GetPosition().y());
	}
}

void TTrackingAction::PostUserTrackingAction(const G4Track* track) {
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->FillNtupleDColumn(10, track->GetPosition().x());
	analysisManager->FillNtupleDColumn(11, track->GetPosition().y());
	analysisManager->FillNtupleDColumn(12, track->GetPosition().z());
	analysisManager->FillNtupleDColumn(13, track->GetMomentum().x());
	analysisManager->FillNtupleDColumn(14, track->GetMomentum().y());
	analysisManager->FillNtupleDColumn(15, track->GetMomentum().z());
	analysisManager->AddNtupleRow();
}