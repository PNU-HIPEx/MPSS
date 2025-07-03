#include "TPhysicsList.hpp"

#include "G4EmStandardPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4HadronElasticPhysicsXS.hh"
#include "G4StoppingPhysics.hh"
#include "G4IonPhysicsXS.hh"
#include "G4IonElasticPhysics.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4eBremsstrahlung.hh"

#include "G4SystemOfUnits.hh"
#include "G4RegionStore.hh"
#include "G4ProcessManager.hh"

TPhysicsList::TPhysicsList() {
	// Electromagnetic Processes
	RegisterPhysics(new G4EmStandardPhysics());
	RegisterPhysics(new G4EmExtraPhysics());

	// Hadronic Processes
	RegisterPhysics(new G4HadronElasticPhysicsXS());
	// RegisterPhysics(new G4HadronInelasticQBBC());
	RegisterPhysics(new G4StoppingPhysics());
	RegisterPhysics(new G4IonPhysicsXS());
	RegisterPhysics(new G4IonElasticPhysics());
	RegisterPhysics(new G4HadronPhysicsQGSP_BERT());

	// Decay Processes
	RegisterPhysics(new G4DecayPhysics());
	RegisterPhysics(new G4RadioactiveDecayPhysics());

	RegisterPhysics(new G4StepLimiterPhysics());
}

void TPhysicsList::SetCuts() {
	SetCutsWithDefault();

	G4Region* region;
	G4String regName;
	G4ProductionCuts* cuts;

	G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(1 * eV, 1 * MeV);

	// Set cuts for tungsten region

	regName = "TungstenRegion";
	region = G4RegionStore::GetInstance()->GetRegion(regName);
	cuts = new G4ProductionCuts;
	cuts->SetProductionCut(0.1 * um, G4ProductionCuts::GetIndex("gamma"));
	cuts->SetProductionCut(0.1 * um, G4ProductionCuts::GetIndex("e-"));
	cuts->SetProductionCut(0.1 * um, G4ProductionCuts::GetIndex("e+"));
	cuts->SetProductionCut(1 * um, G4ProductionCuts::GetIndex("proton"));
	region->SetProductionCuts(cuts);
}

void TPhysicsList::ConstructProcess() {
	G4VModularPhysicsList::ConstructProcess();
}