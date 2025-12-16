#ifndef __TSTEPPINGACTION__
#define __TSTEPPINGACTION__

#include "G4UserSteppingAction.hh"

class TEventAction;
class TTrackingAction;
class G4LogicalVolume;
class G4Step;

class TSteppingAction : public G4UserSteppingAction {
public:
	TSteppingAction(TEventAction* eventAction, TTrackingAction* trackingAction);
	~TSteppingAction() override = default;
private:
	TEventAction* fEventAction = nullptr;
	TTrackingAction* fTrackingAction = nullptr;
	G4LogicalVolume* mWorldLogical = nullptr;
	G4LogicalVolume* mDetectorLogical = nullptr;
	G4LogicalVolume* mCollimatorLogical = nullptr;
public:
	void UserSteppingAction(const G4Step*) override;

	void setDetectors();
};

#endif