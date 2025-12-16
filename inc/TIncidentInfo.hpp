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
public:
	void setIncidentInfo(G4ThreeVector position, G4ThreeVector momentum, G4double kineticEnergy, G4double energyDeposit);
	std::vector<int> getFamilyID();
	void addEnergyDeposit(G4int trackID, G4double energyDeposit);
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

	G4double getFinalPositionX() { return mFinalPosition.x(); }
	G4double getFinalPositionY() { return mFinalPosition.y(); }
	G4double getFinalPositionZ() { return mFinalPosition.z(); }
	G4int getFinalVolumeID() { return mFinalVolumeID; }

};

#endif