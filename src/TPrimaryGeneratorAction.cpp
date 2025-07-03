#include "TPrimaryGeneratorAction.hpp"

#include<string>

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4GeneralParticleSource.hh"
#include "Randomize.hh"

#include "G4SystemOfUnits.hh"

TPrimaryGeneratorAction::TPrimaryGeneratorAction() : G4VUserPrimaryGeneratorAction() {
	fParticleGun = new G4ParticleGun(1);

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* particle = particleTable->FindParticle("e-");
	fParticleGun->SetParticleDefinition(particle);
}

TPrimaryGeneratorAction::TPrimaryGeneratorAction(const KEI::TConfigFile& config) : G4VUserPrimaryGeneratorAction(), mConfig(config) {
	mParticleName = mConfig.getConfig("ENVIRONMENT").getValue<std::string>("PARTICLE_NAME");

	mParticleEnergy = mConfig.getConfig("ENVIRONMENT").getValue<double>("PARTICLE_ENERGY") * keV;

	fParticleGun = new G4ParticleGun(1);

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* particle = particleTable->FindParticle(mParticleName);
	fParticleGun->SetParticleDefinition(particle);
}

TPrimaryGeneratorAction::~TPrimaryGeneratorAction() {
	delete fParticleGun;
}

void TPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {

	// Default energy if mAlphaEnergy is empty
	fParticleGun->SetParticleEnergy(mParticleEnergy);

	G4double randomX = 1 * mm;
	G4double randomY = 1 * mm;
	G4double randomZ = 0;

	fParticleGun->SetParticlePosition(G4ThreeVector(randomX, randomY, randomZ));

	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));

	fParticleGun->GeneratePrimaryVertex(anEvent);
}

const G4ParticleGun* TPrimaryGeneratorAction::GetParticleGun() const {
	return fParticleGun;
}