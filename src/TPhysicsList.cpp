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

TPhysicsList::TPhysicsList(const KEI::TConfigFile& config) : mConfig(config) {
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

	std::array<double, 2> energyRange = {1, 1000000};
	if ( mConfig.getConfig("PHYSICS").hasKey("ENERGY_RANGE") ) {
		energyRange = mConfig.getConfig("PHYSICS").getValue<double, 2>("ENERGY_RANGE");
	}
	G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(energyRange[0] * eV, energyRange[1] * eV);

	if ( mConfig.getConfig("PHYSICS").hasKey("COLLIMATOR_CUT") ) {
		regName = "CollimatorRegion";
		region = G4RegionStore::GetInstance()->GetRegion(regName);
		cuts = new G4ProductionCuts;
		std::array<double, 4> collimatorCuts = mConfig.getConfig("PHYSICS").getValue<double, 4>("COLLIMATOR_CUT");
		cuts->SetProductionCut(collimatorCuts[0] * mm, G4ProductionCuts::GetIndex("gamma"));
		cuts->SetProductionCut(collimatorCuts[1] * mm, G4ProductionCuts::GetIndex("e-"));
		cuts->SetProductionCut(collimatorCuts[2] * mm, G4ProductionCuts::GetIndex("e+"));
		cuts->SetProductionCut(collimatorCuts[3] * mm, G4ProductionCuts::GetIndex("proton"));
		region->SetProductionCuts(cuts);
	}

	if ( mConfig.getConfig("PHYSICS").hasKey("ALPIDE_CUT") ) {
		regName = "ALPIDERegion";
		region = G4RegionStore::GetInstance()->GetRegion(regName);
		cuts = new G4ProductionCuts;
		std::array<double, 4> alpideCuts = mConfig.getConfig("PHYSICS").getValue<double, 4>("ALPIDE_CUT");
		cuts->SetProductionCut(alpideCuts[0] * um, G4ProductionCuts::GetIndex("gamma"));
		cuts->SetProductionCut(alpideCuts[1] * um, G4ProductionCuts::GetIndex("e-"));
		cuts->SetProductionCut(alpideCuts[2] * um, G4ProductionCuts::GetIndex("e+"));
		cuts->SetProductionCut(alpideCuts[3] * um, G4ProductionCuts::GetIndex("proton"));
		region->SetProductionCuts(cuts);
	}

	if ( mConfig.getConfig("PHYSICS").hasKey("SHIELD_CUT") ) {
		regName = "ShieldRegion";
		region = G4RegionStore::GetInstance()->GetRegion(regName);
		cuts = new G4ProductionCuts;
		std::array<double, 4> alpideCuts = mConfig.getConfig("PHYSICS").getValue<double, 4>("SHIELD_CUT");
		cuts->SetProductionCut(alpideCuts[0] * um, G4ProductionCuts::GetIndex("gamma"));
		cuts->SetProductionCut(alpideCuts[1] * um, G4ProductionCuts::GetIndex("e-"));
		cuts->SetProductionCut(alpideCuts[2] * um, G4ProductionCuts::GetIndex("e+"));
		cuts->SetProductionCut(alpideCuts[3] * um, G4ProductionCuts::GetIndex("proton"));
		region->SetProductionCuts(cuts);
	}

	if ( mConfig.getConfig("PHYSICS").hasKey("CASE_CUT") ) {
		regName = "CaseRegion";
		region = G4RegionStore::GetInstance()->GetRegion(regName);
		cuts = new G4ProductionCuts;
		std::array<double, 4> alpideCuts = mConfig.getConfig("PHYSICS").getValue<double, 4>("CASE_CUT");
		cuts->SetProductionCut(alpideCuts[0] * um, G4ProductionCuts::GetIndex("gamma"));
		cuts->SetProductionCut(alpideCuts[1] * um, G4ProductionCuts::GetIndex("e-"));
		cuts->SetProductionCut(alpideCuts[2] * um, G4ProductionCuts::GetIndex("e+"));
		cuts->SetProductionCut(alpideCuts[3] * um, G4ProductionCuts::GetIndex("proton"));
		region->SetProductionCuts(cuts);
	}
}

void TPhysicsList::ConstructProcess() {
	G4VModularPhysicsList::ConstructProcess();
}