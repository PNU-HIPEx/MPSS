#ifndef __TPRIMARYGENERATORACTION__
#define __TPRIMARYGENERATORACTION__

#include "G4VUserPrimaryGeneratorAction.hh"

#include "G4String.hh"

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

	G4String mParticleName;
	G4double mParticleEnergy;

public:
	void GeneratePrimaries(G4Event*) override;

	// const G4GeneralParticleSource* GetParticleGun() const;
	const G4ParticleGun* GetParticleGun() const;
};

#endif