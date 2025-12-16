#include "TTrackingAction.hpp"
#include "G4AnalysisManager.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "TAnalysisHelper.hpp"
#include "TIncidentInfo.hpp"
#include "TDetectorConstruction.hpp"
#include "TEventAction.hpp"
#include "G4Step.hh"

TTrackingAction::TTrackingAction(TEventAction* eventAction) : fEventAction(eventAction) { }

void TTrackingAction::PreUserTrackingAction(const G4Track* track) {
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	if ( track->GetParentID() == 0 ) {
		TAnalysisHelper::FillH3("P_INI_POS_XYZ", track->GetPosition().x() / CLHEP::mm, track->GetPosition().y() / CLHEP::mm, track->GetPosition().z() / CLHEP::mm);

		TAnalysisHelper::FillH2("P_INI_ANG_TP", track->GetMomentum().theta() / CLHEP::deg, track->GetMomentum().phi() / CLHEP::deg);

		TAnalysisHelper::FillH1("P_INI_KE", track->GetKineticEnergy() / CLHEP::keV);
	} else {
		TAnalysisHelper::FillH3("S_INI_POS_XYZ", track->GetPosition().x(), track->GetPosition().y(), track->GetPosition().z());
		TAnalysisHelper::FillH2("S_INI_ANG_TP", track->GetMomentum().theta() / CLHEP::deg, track->GetMomentum().phi() / CLHEP::deg);
		TAnalysisHelper::FillH1("S_INI_KE", track->GetKineticEnergy() / CLHEP::keV);

		// Secondary Particle 들의 종류와 개수를 추적하는 코드 작성해야함
	}
	mIncident = false;
	mEnergyDeposit = 0.;
}

void TTrackingAction::PostUserTrackingAction(const G4Track* track) {
	if ( mIncident ) {
		TIncidentInfo* incident = new TIncidentInfo(track);
		incident->setIncidentInfo(mIncidentPosition, mIncidentMomentum, mIncidentKineticEnergy, mEnergyDeposit);
		fEventAction->addIncidentParticle(incident);
	} else if ( mEnergyDeposit > 0. ) {
		for ( TIncidentInfo* incident : fEventAction->getIncidentInfo() ) {
			std::vector<int> familyID = incident->getFamilyID();
			if ( std::find(familyID.begin(), familyID.end(), track->GetParentID()) != familyID.end() ) {
				incident->addEnergyDeposit(track->GetTrackID(), mEnergyDeposit);
			}
		}
	}
}

void TTrackingAction::addEnergyDeposit(G4double energyDeposit) {
	mEnergyDeposit += energyDeposit;
}

void TTrackingAction::setIncidentParticle(const G4Step* step) {
	mIncident = true;
	mIncidentPosition = step->GetPreStepPoint()->GetPosition();
	mIncidentMomentum = step->GetPreStepPoint()->GetMomentum();
	mIncidentKineticEnergy = step->GetPreStepPoint()->GetKineticEnergy();
}
