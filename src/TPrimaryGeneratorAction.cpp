#include "TPrimaryGeneratorAction.hpp"

#include<string>

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4GeneralParticleSource.hh"
#include "Randomize.hh"
#include<csv.h>

#include "G4SystemOfUnits.hh"

TPrimaryGeneratorAction::TPrimaryGeneratorAction() : G4VUserPrimaryGeneratorAction() {
	fParticleGun = new G4ParticleGun(1);

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* particle = particleTable->FindParticle("e-");
	fParticleGun->SetParticleDefinition(particle);
}

TPrimaryGeneratorAction::TPrimaryGeneratorAction(const KEI::TConfigFile& config) : G4VUserPrimaryGeneratorAction(), mConfig(config) {
	fParticleGun = new G4ParticleGun(1);

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	if ( mConfig.getConfig("ENVIRONMENT").getValue<std::string>("SOURCE_PARTICLE") == "alpha" ) {
		G4ParticleDefinition* particle = particleTable->FindParticle("alpha");
		fParticleGun->SetParticleDefinition(particle);
	} else if ( mConfig.getConfig("ENVIRONMENT").getValue<std::string>("SOURCE_PARTICLE") == "gamma" ) {
		G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
		fParticleGun->SetParticleDefinition(particle);
	} else {
		std::cerr << "Invalid source particle." << std::endl;
		std::cerr << "Particle could be set in the configuration file with the key SOURCE_PARTICLE.in ENVIRONMENT" << std::endl;
		exit(1);
	}
	setEnergyDistribution(mConfig.getConfig("ENVIRONMENT").getValue<std::string>("SOURCE_PARTICLE"));
}

TPrimaryGeneratorAction::~TPrimaryGeneratorAction() {
	delete fParticleGun;
}

void TPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
	// Select a random energy based on intensity

	std::vector<double> cumulativeDistribution;
	double sum = 0.0;
	for ( const auto& entry : mEnergy ) {
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
	double selectedEnergy = G4RandGauss::shoot(mEnergy[index].energy, mEnergy[index].energyUncertainty);

	fParticleGun->SetParticleEnergy(selectedEnergy * keV);

	G4double collimatorLength = mConfig.getConfig("ENVIRONMENT").getValue<double>("COLLIMATOR_LENGTH") * mm;
	G4double distanceBetweenALPIDEAndCollimator = mConfig.getConfig("ENVIRONMENT").hasKey("DISTANCE_ALPIDE_COLLIMATOR") ? mConfig.getConfig("ENVIRONMENT").getValue<double>("DISTANCE_ALPIDE_COLLIMATOR") * mm : 2. * mm;
	G4double distanceBetweenSourceAndCollimator = mConfig.getConfig("ENVIRONMENT").hasKey("DISTANCE_SOURCE_COLLIMATOR") ? mConfig.getConfig("ENVIRONMENT").getValue<double>("DISTANCE_SOURCE_COLLIMATOR") * mm : .5 * mm;
	G4double centerPositionZ = (collimatorLength + distanceBetweenALPIDEAndCollimator + distanceBetweenSourceAndCollimator) * mm;
	G4double centerPosition[3] = {0, 0, centerPositionZ};
	G4double radius = mConfig.getConfig("ENVIRONMENT").getValue<double>("SOURCE_RADIUS") * mm;

	G4double r = radius * std::sqrt(G4UniformRand());
	G4double theta = 2 * M_PI * G4UniformRand();
	G4double randomX = centerPosition[0] + r * std::cos(theta);
	G4double randomY = centerPosition[1] + r * std::sin(theta);
	G4double randomZ = centerPosition[2];

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

void TPrimaryGeneratorAction::setEnergyDistribution(std::string_view particle) {
	std::string csvPath;
	if ( particle == "alpha" ) {
		csvPath = mConfig.getConfig("FILE").getValue<std::string>("ALPHA_ENERGY");
	} else if ( particle == "gamma" ) {
		csvPath = mConfig.getConfig("FILE").getValue<std::string>("GAMMA_ENERGY");
	} else {
		std::cerr << "Invalid particle." << std::endl;
		exit(1);
	}
	io::CSVReader<4> in(csvPath);
	in.read_header(io::ignore_extra_column, "energy", "energy_uncertainty", "intensity", "intensity_uncertainty");
	double energy, energyUncertainty, intensity, intensityUncertainty;
	while ( in.read_row(energy, energyUncertainty, intensity, intensityUncertainty) ) {
		mEnergy.push_back({energy, energyUncertainty, intensity, intensityUncertainty});
	}
}