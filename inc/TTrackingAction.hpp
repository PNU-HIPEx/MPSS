#ifndef __TTACKINGACTION__
#define __TTACKINGACTION__

#include "G4UserTrackingAction.hh"
#include "G4ThreeVector.hh"

class TEventAction;
class G4Step;

class TTrackingAction : public G4UserTrackingAction {
public:
	TTrackingAction(TEventAction* eventAction);
	~TTrackingAction() override = default;
private:
	TEventAction* fEventAction = nullptr;
	G4bool mIncident;
	G4ThreeVector mIncidentPosition, mIncidentMomentum;
	G4double mIncidentKineticEnergy;

	G4double mEnergyDeposit;
	G4int mNStep;
	G4double mDepositMeanX;
	G4double mDepositMeanY;
	G4double mDepositMeanZ;
	G4double mDepositStdDevX;
	G4double mDepositStdDevY;
	G4double mDepositStdDevZ;
public:
	void PreUserTrackingAction(const G4Track*) override;
	void PostUserTrackingAction(const G4Track*) override;

	void addEnergyDeposit(G4double energyDeposit, G4double meanX, G4double meanY, G4double meanZ);
	void setIncidentParticle(const G4Step* step);
};

#endif