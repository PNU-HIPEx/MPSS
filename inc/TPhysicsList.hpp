#ifndef __TPHYSICSLIST__
#define __TPHYSICSLIST__

#include "G4VModularPhysicsList.hh"

class TPhysicsList : public G4VModularPhysicsList {
public:
	TPhysicsList();
private:
	void ConstructProcess() override;

	void SetCuts() override;
};

#endif