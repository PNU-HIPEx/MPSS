#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"

#include "TSteppingAction.hpp"
#include "TDetectorConstruction.hpp"
#include "TIncidentInfo.hpp"
#include "TEventAction.hpp"
#include "TTrackingAction.hpp"

TSteppingAction::TSteppingAction(TEventAction* eventAction, TTrackingAction* trackingAction) : G4UserSteppingAction(), fEventAction(eventAction), fTrackingAction(trackingAction) { }

void TSteppingAction::UserSteppingAction(const G4Step* step) {
	G4Track* track = step->GetTrack();

	G4LogicalVolume* preVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
	G4LogicalVolume* initVolume = step->GetTrack()->GetOriginTouchableHandle()->GetVolume()->GetLogicalVolume();

	setDetectors();

	if ( preVolume == mDetectorLogical ) {
		// Mark as incident if the track enters detector from outside
		if ( step->IsFirstStepInVolume() && initVolume != mDetectorLogical ) {
			fTrackingAction->setIncidentParticle(step);
		}
		// Accumulate energy deposit per track when inside detector
		const G4double edep = step->GetTotalEnergyDeposit();
		if ( edep > 0.0 ) {
			fTrackingAction->addEnergyDeposit(edep);
		}
	}
}

void TSteppingAction::setDetectors() {
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
	if ( mShieldVerticalLogical == nullptr ) {
		mShieldVerticalLogical = detectorConstruction->getShieldVerticalLogical();
	}
	if ( mShieldLeftLogical == nullptr ) {
		mShieldLeftLogical = detectorConstruction->getShieldLeftLogical();
	}
	if ( mShieldRightLogical == nullptr ) {
		mShieldRightLogical = detectorConstruction->getShieldRightLogical();
	}
	if ( mCaseVerticalLogical == nullptr ) {
		mCaseVerticalLogical = detectorConstruction->getCaseVerticalLogical();
	}
	if ( mCaseLeftLogical == nullptr ) {
		mCaseLeftLogical = detectorConstruction->getCaseLeftLogical();
	}
	if ( mCaseRightLogical == nullptr ) {
		mCaseRightLogical = detectorConstruction->getCaseRightLogical();
	}
}