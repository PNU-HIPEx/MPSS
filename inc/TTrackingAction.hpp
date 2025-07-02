#ifndef __TTACKINGACTION__
#define __TTACKINGACTION__

#include "G4UserTrackingAction.hh"

class TTrackingAction : public G4UserTrackingAction {
public:
	TTrackingAction() = default;
	~TTrackingAction() override = default;

	void PreUserTrackingAction(const G4Track*) override;
	void PostUserTrackingAction(const G4Track*) override;
private:
};

#endif