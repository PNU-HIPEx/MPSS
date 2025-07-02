#ifndef __TEVENTACTION__
#define __TEVENTACTION__

#include "G4UserEventAction.hh"

class TRunAction;
class G4Event;

class TEventAction : public G4UserEventAction {
public:
	TEventAction(TRunAction* runAction);
	~TEventAction() override = default;

	void BeginOfEventAction(const G4Event*) override;
	void EndOfEventAction(const G4Event*) override;

private:
	TRunAction* fRunAction;
};

#endif