#ifndef __TINCIDENTINFO__
#define __TINCIDENTINFO__

#include "G4AnalysisManager.hh"

class G4Track;

class TIncidentInfo {
public:
	TIncidentInfo(const G4Track* track);
private:
	G4int mTrackID, mParentID, mParticleID;
	std::vector<int> mFamilyID;

	G4ThreeVector mVertexPosition, mVertexMomentum;
	G4double mVertexKineticEnergy;
	G4int mVertexVolumeID;

	G4ThreeVector mIncidentPosition, mIncidentMomentum;
	G4double mIncidentKineticEnergy;

	G4ThreeVector mFinalPosition;
	G4int mFinalVolumeID;

	G4double mEnergyDeposit = 0.0;
	G4int mNStep;
	G4double mDepositMeanX;
	G4double mDepositMeanY;
	G4double mDepositMeanZ;
	G4double mDepositStdDevX;
	G4double mDepositStdDevY;
	G4double mDepositStdDevZ;
public:
	void setIncidentInfo(G4ThreeVector position, G4ThreeVector momentum, G4double kineticEnergy, G4double energyDeposit, G4int nStep, G4double depositMeanX, G4double depositMeanY, G4double depositStdDevX, G4double depositStdDevY);
	std::vector<int> getFamilyID();
	void addEnergyDeposit(G4int trackID, G4double energyDeposit, G4int nStep, G4double depositMeanX, G4double depositMeanY, G4double depositMeanZ, G4double depositStdDevX, G4double depositStdDevY, G4double depositStdDevZ);
	G4int getTrackID() { return mTrackID; }
	G4int getParentID() { return mParentID; }
	G4int getParticleID() { return mParticleID; }
	G4double getVertexPositionX() { return mVertexPosition.x(); }
	G4double getVertexPositionY() { return mVertexPosition.y(); }
	G4double getVertexPositionZ() { return mVertexPosition.z(); }
	G4double getVertexMomentumX() { return mVertexMomentum.x(); }
	G4double getVertexMomentumY() { return mVertexMomentum.y(); }
	G4double getVertexMomentumZ() { return mVertexMomentum.z(); }
	G4double getVertexKineticEnergy() { return mVertexKineticEnergy; }
	G4int getVertexVolumeID() { return mVertexVolumeID; }

	G4double getIncidentPositionX() { return mIncidentPosition.x(); }
	G4double getIncidentPositionY() { return mIncidentPosition.y(); }
	G4double getIncidentPositionZ() { return mIncidentPosition.z(); }
	G4double getIncidentMomentumX() { return mIncidentMomentum.x(); }
	G4double getIncidentMomentumY() { return mIncidentMomentum.y(); }
	G4double getIncidentMomentumZ() { return mIncidentMomentum.z(); }
	G4double getIncidentKineticEnergy() { return mIncidentKineticEnergy; }
	G4double getEnergyDeposit() { return mEnergyDeposit; }
	G4double getNStep() { return mNStep; }
	G4double getDepositMeanX() { return mDepositMeanX / mNStep; }
	G4double getDepositMeanY() { return mDepositMeanY / mNStep; }
	G4double getDepositStdDevX() { return std::sqrt((mDepositStdDevX / mNStep) - std::pow(mDepositMeanX / mNStep, 2)); }
	G4double getDepositStdDevY() { return std::sqrt((mDepositStdDevY / mNStep) - std::pow(mDepositMeanY / mNStep, 2)); }

	G4double getFinalPositionX() { return mFinalPosition.x(); }
	G4double getFinalPositionY() { return mFinalPosition.y(); }
	G4double getFinalPositionZ() { return mFinalPosition.z(); }
	G4int getFinalVolumeID() { return mFinalVolumeID; }


};

#endif