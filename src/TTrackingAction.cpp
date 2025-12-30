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
	mNStep = 0;
	mDepositMeanX = 0.;
	mDepositMeanY = 0.;
	mDepositStdDevX = 0.;
	mDepositStdDevY = 0.;
}

void TTrackingAction::PostUserTrackingAction(const G4Track* track) {
	if ( mIncident ) {
		TIncidentInfo* incident = new TIncidentInfo(track);
		incident->setIncidentInfo(mIncidentPosition, mIncidentMomentum, mIncidentKineticEnergy, mEnergyDeposit, mNStep, mDepositMeanX, mDepositMeanY, mDepositStdDevX, mDepositStdDevY);
		fEventAction->addIncidentParticle(incident);
	} else if ( mEnergyDeposit > 0. ) {
		for ( TIncidentInfo* incident : fEventAction->getIncidentInfo() ) {
			std::vector<int> familyID = incident->getFamilyID();
			if ( std::find(familyID.begin(), familyID.end(), track->GetParentID()) != familyID.end() ) {
				incident->addEnergyDeposit(track->GetTrackID(), mEnergyDeposit, mNStep, mDepositMeanX, mDepositMeanY, mDepositMeanZ, mDepositStdDevX, mDepositStdDevY, mDepositStdDevZ);
				break;
			}
		}
	}
}

void TTrackingAction::addEnergyDeposit(G4double energyDeposit, G4double meanX, G4double meanY, G4double meanZ) {
	mEnergyDeposit += energyDeposit;
	mNStep++;
	mDepositMeanX += meanX;
	mDepositMeanY += meanY;
	mDepositMeanZ += meanZ;
	mDepositStdDevX += meanX * meanX;
	mDepositStdDevY += meanY * meanY;
	mDepositStdDevZ += meanZ * meanZ;
}

void TTrackingAction::setIncidentParticle(const G4Step* step) {
	mIncident = true;
	mIncidentPosition = step->GetPreStepPoint()->GetPosition();
	mIncidentMomentum = step->GetPreStepPoint()->GetMomentum();
	mIncidentKineticEnergy = step->GetPreStepPoint()->GetKineticEnergy();
}
