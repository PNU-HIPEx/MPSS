#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"

#include "TSteppingAction.hpp"
#include "TDetectorConstruction.hpp"

TSteppingAction::TSteppingAction(TEventAction* eventAction) : G4UserSteppingAction(), fEventAction(eventAction) { }

void TSteppingAction::UserSteppingAction(const G4Step* step) {
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	G4LogicalVolume* preVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
	G4LogicalVolume* initVolume = step->GetTrack()->GetOriginTouchableHandle()->GetVolume()->GetLogicalVolume();
	const TDetectorConstruction* detectorConstruction = static_cast<const TDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	if ( mWorldLogical == nullptr ) {
		mWorldLogical = detectorConstruction->getWorldLogical();
	}
	if ( mDetectorLogical == nullptr ) {
		mDetectorLogical = detectorConstruction->getDetectorLogical();
	}
	if ( mCollimatorLogical == nullptr ) {
		mCollimatorLogical = detectorConstruction->getCollimatorLogical();
	}

	if ( preVolume == mDetectorLogical && step->IsFirstStepInVolume() && initVolume != mDetectorLogical) {
			analysisManager->FillNtupleIColumn(1, 0, G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID());
			analysisManager->FillNtupleIColumn(1, 1, step->GetTrack()->GetTrackID());
			analysisManager->FillNtupleIColumn(1, 2, step->GetTrack()->GetParentID());
			analysisManager->FillNtupleIColumn(1, 3, step->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
			analysisManager->FillNtupleDColumn(1, 4, step->GetPreStepPoint()->GetPosition().x());
			analysisManager->FillNtupleDColumn(1, 5, step->GetPreStepPoint()->GetPosition().y());
			analysisManager->FillNtupleDColumn(1, 6, step->GetPreStepPoint()->GetPosition().z());
			analysisManager->FillNtupleDColumn(1, 7, step->GetPreStepPoint()->GetMomentum().x());
			analysisManager->FillNtupleDColumn(1, 8, step->GetPreStepPoint()->GetMomentum().y());
			analysisManager->FillNtupleDColumn(1, 9, step->GetPreStepPoint()->GetMomentum().z());
			analysisManager->AddNtupleRow(1);
	}
}