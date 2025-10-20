#ifndef __TRUNACTION__
#define __TRUNACTION__

#include "G4UserRunAction.hh"
#include "TConfig.hpp"

class TRunAction : public G4UserRunAction {
public:
	TRunAction() = default;
	TRunAction(const KEI::TConfigFile& config);
	~TRunAction() override = default;

	void BeginOfRunAction(const G4Run*) override;
	void EndOfRunAction(const G4Run*) override;
private:
	KEI::TConfigFile mConfig;
};

#endif