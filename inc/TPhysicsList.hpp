#ifndef __TPHYSICSLIST__
#define __TPHYSICSLIST__

#include "G4VModularPhysicsList.hh"
#include "TConfig.hpp"

class TPhysicsList : public G4VModularPhysicsList {
public:
	TPhysicsList();
	TPhysicsList(const KEI::TConfigFile& config);
private:
	KEI::TConfigFile mConfig;

	void ConstructProcess() override;

	void SetCuts() override;
};

#endif