#ifndef __TEVENTACTION__
#define __TEVENTACTION__

#include "G4UserEventAction.hh"
#include<vector>
#include<unordered_map>

#include "TIncidentInfo.hpp"

class TRunAction;
class G4Event;

class TEventAction : public G4UserEventAction {
public:
	TEventAction(TRunAction* runAction);
	~TEventAction() override;

	void BeginOfEventAction(const G4Event*) override;
	void EndOfEventAction(const G4Event*) override;

private:
	TRunAction* fRunAction;
	std::vector<TIncidentInfo*> mIncidentParticle;
public:
	void addIncidentParticle(TIncidentInfo* incident);
	std::vector<TIncidentInfo*> getIncidentInfo();
};

#endif