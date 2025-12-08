#include "TTrackingAction.hpp"
#include "G4AnalysisManager.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "TAnalysisHelper.hpp"
#include "TIncidentInfo.hpp"

void TTrackingAction::PreUserTrackingAction(const G4Track* track) {
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	if ( track->GetParentID() == 0 ) {
		TAnalysisHelper::FillH3("P_INI_POS_XYZ", track->GetPosition().x()/CLHEP::mm, track->GetPosition().y()/CLHEP::mm, track->GetPosition().z()/CLHEP::mm);
		
		TAnalysisHelper::FillH2("P_INI_ANG_TP", track->GetMomentum().theta()/CLHEP::deg, track->GetMomentum().phi()/CLHEP::deg);

		TAnalysisHelper::FillH1("P_INI_KE", track->GetKineticEnergy()/CLHEP::keV);
	} else {
		TAnalysisHelper::FillH3("S_INI_POS_XYZ", track->GetPosition().x(), track->GetPosition().y(), track->GetPosition().z());
		TAnalysisHelper::FillH2("S_INI_ANG_TP", track->GetMomentum().theta()/CLHEP::deg, track->GetMomentum().phi()/CLHEP::deg);
		TAnalysisHelper::FillH1("S_INI_KE", track->GetKineticEnergy()/CLHEP::keV);

		// Secondary Particle 들의 종류와 개수를 추적하는 코드 작성해야함
	}
}

void TTrackingAction::PostUserTrackingAction(const G4Track* track) {
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	TIncidentInfo* info = static_cast<TIncidentInfo*>(track->GetUserInformation());

	if (info && info->mIsIncident) {
		
		analysisManager->FillNtupleIColumn(0, G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID());
		analysisManager->FillNtupleIColumn(1, track->GetTrackID());
		analysisManager->FillNtupleIColumn(2, track->GetParentID());
		analysisManager->FillNtupleIColumn(3, track->GetParticleDefinition()->GetPDGEncoding());

		G4ThreeVector vertexPosition = track->GetVertexPosition();
		analysisManager->FillNtupleDColumn(4, vertexPosition.x() / CLHEP::mm);
		analysisManager->FillNtupleDColumn(5, vertexPosition.y() / CLHEP::mm);
		analysisManager->FillNtupleDColumn(6, vertexPosition.z() / CLHEP::mm);
		G4ThreeVector vertexMomentum = track->GetVertexMomentumDirection();
		analysisManager->FillNtupleDColumn(7, vertexMomentum.x() / CLHEP::keV);
		analysisManager->FillNtupleDColumn(8, vertexMomentum.y() / CLHEP::keV);
		analysisManager->FillNtupleDColumn(9, vertexMomentum.z() / CLHEP::keV);
		
		analysisManager->FillNtupleDColumn(10, track->GetVertexKineticEnergy() / CLHEP::keV);

		analysisManager->FillNtupleDColumn(11, info->mIncidentPosition.x() / CLHEP::mm);
		analysisManager->FillNtupleDColumn(12, info->mIncidentPosition.y() / CLHEP::mm);
		analysisManager->FillNtupleDColumn(13, info->mIncidentPosition.z() / CLHEP::mm);
		analysisManager->FillNtupleDColumn(14, info->mIncidentMomentum.x() / CLHEP::keV);
		analysisManager->FillNtupleDColumn(15, info->mIncidentMomentum.y() / CLHEP::keV);
		analysisManager->FillNtupleDColumn(16, info->mIncidentMomentum.z() / CLHEP::keV);
		analysisManager->FillNtupleDColumn(17, info->mIncidentKineticEnergy / CLHEP::keV);
		analysisManager->FillNtupleDColumn(18, track->GetPosition().x() / CLHEP::mm);
		analysisManager->FillNtupleDColumn(19, track->GetPosition().y() / CLHEP::mm);
		analysisManager->FillNtupleDColumn(20, track->GetPosition().z() / CLHEP::mm);
		analysisManager->AddNtupleRow();
	}
}