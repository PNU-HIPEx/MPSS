#ifndef __TPRIMARYGENERATORACTION__
#define __TPRIMARYGENERATORACTION__

#include "G4VUserPrimaryGeneratorAction.hh"
// #include "G4GeneralParticleSource.hh"
// #include "G4ParticleGun.hh"
// #include "TAnalysisManager.hpp"

// #include<filesystem>

#include "TConfig.h"

class G4GeneralParticleSource;
class G4ParticleGun;

class TPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
	TPrimaryGeneratorAction();
	TPrimaryGeneratorAction(const KEI::TConfigFile& config);
	~TPrimaryGeneratorAction() override;
private:
	G4ParticleGun* fParticleGun;

	KEI::TConfigFile mConfig;
	// G4GeneralParticleSource* fParticleGun;

	struct AlphaEnergyEntry {
		double energy;
		double energyUncertainty;
		double intensity;
		double intensityUncertainty;
	};

	std::vector<AlphaEnergyEntry> mAlphaEnergy;
public:
	void GeneratePrimaries(G4Event*) override;

	// const G4GeneralParticleSource* GetParticleGun() const;
	const G4ParticleGun* GetParticleGun() const;
};

#endif