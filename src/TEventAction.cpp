#include "TEventAction.hpp"

#include "TRunAction.hpp"
#include "TAnalysisManager.hpp"
#include "G4AnalysisManager.hh"
#include "G4Event.hh"

TEventAction::TEventAction(TRunAction* runAction) : G4UserEventAction(), fRunAction(runAction) { }

void TEventAction::BeginOfEventAction(const G4Event* event) {
	ProgressBar::incrementGlobal();
}

void TEventAction::EndOfEventAction(const G4Event* event) {
}