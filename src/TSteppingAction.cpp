#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"

#include "TSteppingAction.hpp"
#include "TDetectorConstruction.hpp"
#include "TIncidentInfo.hpp"

TSteppingAction::TSteppingAction(TEventAction* eventAction) : G4UserSteppingAction(), fEventAction(eventAction) { }

void TSteppingAction::UserSteppingAction(const G4Step* step) {
	G4Track* track = step->GetTrack();
	TIncidentInfo* info = static_cast<TIncidentInfo*>(track->GetUserInformation());
    if (!info) {
        info = new TIncidentInfo();
        track->SetUserInformation(info);
    }

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
		info->mIsIncident = true;		
		info->mIncidentPosition = step->GetPreStepPoint()->GetPosition();
		info->mIncidentMomentum = step->GetPreStepPoint()->GetMomentum();
		info->mIncidentKineticEnergy = step->GetPreStepPoint()->GetKineticEnergy();
	}
}