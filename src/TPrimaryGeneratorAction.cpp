#include "TPrimaryGeneratorAction.hpp"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4GeneralParticleSource.hh"
#include "Randomize.hh"

#include "G4SystemOfUnits.hh"

TPrimaryGeneratorAction::TPrimaryGeneratorAction() : G4VUserPrimaryGeneratorAction() {
	fParticleGun = new G4ParticleGun(1);

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* particle = particleTable->FindParticle("alpha");
	fParticleGun->SetParticleDefinition(particle);
}

TPrimaryGeneratorAction::TPrimaryGeneratorAction(const KEI::TConfigFile& config) : G4VUserPrimaryGeneratorAction(), mConfig(config) {
	fParticleGun = new G4ParticleGun(1);

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* particle = particleTable->FindParticle("alpha");
	fParticleGun->SetParticleDefinition(particle);
}

TPrimaryGeneratorAction::~TPrimaryGeneratorAction() {
	delete fParticleGun;
}

void TPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
	// Select a random energy based on intensity
	if ( !mAlphaEnergy.empty() ) {
		// Create cumulative distribution
		std::vector<double> cumulativeDistribution;
		double sum = 0.0;
		for ( const auto& entry : mAlphaEnergy ) {
			sum += entry.intensity;
			cumulativeDistribution.push_back(sum);
		}

		// Normalize cumulative distribution
		for ( auto& value : cumulativeDistribution ) {
			value /= sum;
		}

		// Generate a random number and find corresponding energy
		double randomValue = G4UniformRand();
		auto it = std::lower_bound(cumulativeDistribution.begin(), cumulativeDistribution.end(), randomValue);
		size_t index = std::distance(cumulativeDistribution.begin(), it);
		double selectedEnergy = G4RandGauss::shoot(mAlphaEnergy[index].energy, mAlphaEnergy[index].energyUncertainty);

		fParticleGun->SetParticleEnergy(selectedEnergy * keV);
	} else {
		// Default energy if mAlphaEnergy is empty
		fParticleGun->SetParticleEnergy(5.4 * MeV);
	}

	G4double randomX = 0;
	G4double randomY = 0;
	G4double randomZ = 0;

	fParticleGun->SetParticlePosition(G4ThreeVector(randomX, randomY, randomZ));

	G4double cosTheta = 2 * G4UniformRand() - 1.0;
	G4double sinTheta = std::sqrt(1 - cosTheta * cosTheta);
	G4double phi = 2 * M_PI * G4UniformRand();
	G4double pX = sinTheta * std::cos(phi);
	G4double pY = sinTheta * std::sin(phi);
	G4double pZ = cosTheta;

	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(pX, pY, pZ));

	fParticleGun->GeneratePrimaryVertex(anEvent);
}

const G4ParticleGun* TPrimaryGeneratorAction::GetParticleGun() const {
	return fParticleGun;
}