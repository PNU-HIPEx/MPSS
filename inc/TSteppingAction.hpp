#ifndef __TSTEPPINGACTION__
#define __TSTEPPINGACTION__

#include "G4UserSteppingAction.hh"

class TEventAction;
class G4LogicalVolume;
class G4Step;

class TSteppingAction : public G4UserSteppingAction {
public:
	TSteppingAction(TEventAction* eventAction);
	~TSteppingAction() override = default;
private:
	TEventAction* fEventAction = nullptr;
public:
	void UserSteppingAction(const G4Step*) override;
};

#endif