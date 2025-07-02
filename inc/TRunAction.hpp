#ifndef __TRUNACTION__
#define __TRUNACTION__

#include "G4UserRunAction.hh"

class TRunAction : public G4UserRunAction {
public:
	TRunAction() = default;
	~TRunAction() override = default;

	void BeginOfRunAction(const G4Run*) override;
	void EndOfRunAction(const G4Run*) override;
};

#endif