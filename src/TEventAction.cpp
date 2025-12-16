#include "TEventAction.hpp"

#include "TRunAction.hpp"
#include "TAnalysisManager.hpp"
#include "G4Event.hh"

TEventAction::TEventAction(TRunAction* runAction) : G4UserEventAction(), fRunAction(runAction) { }

TEventAction::~TEventAction() { }

void TEventAction::BeginOfEventAction(const G4Event* event) {
	ProgressBar::incrementGlobal();
	mIncidentParticle.clear();
}

void TEventAction::EndOfEventAction(const G4Event* event) {
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	for ( TIncidentInfo* incident : mIncidentParticle ) {
		analysisManager->FillNtupleIColumn(0, event->GetEventID());
		analysisManager->FillNtupleIColumn(1, incident->getTrackID());
		analysisManager->FillNtupleIColumn(2, incident->getParentID());
		analysisManager->FillNtupleIColumn(3, incident->getParticleID());

		analysisManager->FillNtupleDColumn(4, incident->getVertexPositionX());
		analysisManager->FillNtupleDColumn(5, incident->getVertexPositionY());
		analysisManager->FillNtupleDColumn(6, incident->getVertexPositionZ());
		analysisManager->FillNtupleDColumn(7, incident->getVertexMomentumX());
		analysisManager->FillNtupleDColumn(8, incident->getVertexMomentumY());
		analysisManager->FillNtupleDColumn(9, incident->getVertexMomentumZ());
		analysisManager->FillNtupleDColumn(10, incident->getVertexKineticEnergy());
		analysisManager->FillNtupleIColumn(11, incident->getVertexVolumeID());

		analysisManager->FillNtupleDColumn(12, incident->getIncidentPositionX());
		analysisManager->FillNtupleDColumn(13, incident->getIncidentPositionY());
		analysisManager->FillNtupleDColumn(14, incident->getIncidentPositionZ());
		analysisManager->FillNtupleDColumn(15, incident->getIncidentMomentumX());
		analysisManager->FillNtupleDColumn(16, incident->getIncidentMomentumY());
		analysisManager->FillNtupleDColumn(17, incident->getIncidentMomentumZ());
		analysisManager->FillNtupleDColumn(18, incident->getIncidentKineticEnergy());
		analysisManager->FillNtupleDColumn(19, incident->getEnergyDeposit());

		analysisManager->FillNtupleDColumn(20, incident->getFinalPositionX());
		analysisManager->FillNtupleDColumn(21, incident->getFinalPositionY());
		analysisManager->FillNtupleDColumn(22, incident->getFinalPositionZ());
		analysisManager->FillNtupleIColumn(23, incident->getFinalVolumeID());
		analysisManager->AddNtupleRow();
	}
}

void TEventAction::addIncidentParticle(TIncidentInfo* incident) {
	mIncidentParticle.push_back(incident);
}

std::vector<TIncidentInfo*> TEventAction::getIncidentInfo() {
	return mIncidentParticle;
}
