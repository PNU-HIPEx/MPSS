#include "TIncidentInfo.hpp"

#include "G4Track.hh"
#include "TVolumeID.hh"

TIncidentInfo::TIncidentInfo(const G4Track* track) {
	mTrackID = track->GetTrackID();
	mParentID = track->GetParentID();
	mParticleID = track->GetParticleDefinition()->GetPDGEncoding();
	mFamilyID.push_back(track->GetTrackID());

	mVertexPosition = track->GetVertexPosition();
	mVertexMomentum = track->GetVertexMomentumDirection();
	mVertexKineticEnergy = track->GetVertexKineticEnergy();
	mVertexVolumeID = getVolumeID(track->GetOriginTouchableHandle()->GetVolume()->GetLogicalVolume());

	mFinalPosition = track->GetPosition();
	mFinalVolumeID = getVolumeID(track->GetTouchableHandle()->GetVolume()->GetLogicalVolume());
}

void TIncidentInfo::setIncidentInfo(G4ThreeVector position, G4ThreeVector momentum, G4double kineticEnergy, G4double energyDeposit, G4int nStep, G4double depositMeanX, G4double depositMeanY, G4double depositStdDevX, G4double depositStdDevY) {
	mIncidentPosition = position;
	mIncidentMomentum = momentum;
	mIncidentKineticEnergy = kineticEnergy;
	mEnergyDeposit = energyDeposit;
	mNStep = nStep;
	mDepositMeanX = depositMeanX;
	mDepositMeanY = depositMeanY;
	mDepositStdDevX = depositStdDevX;
	mDepositStdDevY = depositStdDevY;
}

std::vector<int> TIncidentInfo::getFamilyID() {
	return mFamilyID;
}

void TIncidentInfo::addEnergyDeposit(G4int trackID, G4double energyDeposit, G4int nStep, G4double depositMeanX, G4double depositMeanY, G4double depositMeanZ, G4double depositStdDevX, G4double depositStdDevY, G4double depositStdDevZ) {
	mFamilyID.push_back(trackID);
	mEnergyDeposit += energyDeposit;
	mNStep = nStep;
	mDepositMeanX = depositMeanX;
	mDepositMeanY = depositMeanY;
	mDepositMeanZ = depositMeanZ;
	mDepositStdDevX = depositStdDevX;
	mDepositStdDevY = depositStdDevY;
	mDepositStdDevZ = depositStdDevZ;
}
