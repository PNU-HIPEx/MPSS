#include "TPrimaryGeneratorAction.hpp"

#include<string>

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4GeneralParticleSource.hh"
#include "Randomize.hh"
#include<csv.h>

#include "G4SystemOfUnits.hh"

#include "config.hpp"
#include <iostream>
#include <filesystem>

const std::filesystem::path sourcePath = SOURCE_DIR;

const std::string amInfoPath = sourcePath / "config/AM241_ALPHA.csv";
const std::string gmInfoPath = sourcePath / "config/AM241_GAMMA.csv";

// 1. 큰 방사선원 -> 중심좌표 0, 6.5 + 소스자체의 거리 , 0 / 반경 = 1.5
// 2. 작은 방사선원 -> 중심좌표 0, 0, +- ( 1 또는 1.5 + 2 +0.5 / 반경 = 2.5

TPrimaryGeneratorAction::TPrimaryGeneratorAction() : G4VUserPrimaryGeneratorAction() {
	// 한번에 하나의 입자를 쏘겠다.
	fParticleGun = new G4ParticleGun(1);
	// G4에서 제공하는 입자표를 particleTable 이라는 이름으로 들고온다.
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	// 위에서 초기화한 particleTable이라는 입자표에서 electron을 particle이라는 이름으로 들고온다. why electron?
	G4ParticleDefinition* particle = particleTable->FindParticle("e-");
	// 위에서 초기화한 particle 이라는 입자(electron)을 쏘아줄 입자로 정한다.
	fParticleGun->SetParticleDefinition(particle);
}

// Constructor overloading 클래스 이름과 같은 이름을 가지면서 인자가 다른 클래스
TPrimaryGeneratorAction::TPrimaryGeneratorAction(const KEI::TConfigFile& config) : G4VUserPrimaryGeneratorAction(), mConfig(config) {
	fParticleGun = new G4ParticleGun(1);
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	if ( mConfig.getConfig("ENVIRONMENT").getValue<std::string>("PARTICLE_NAME") == "alpha" ) {
		G4ParticleDefinition* particle = particleTable->FindParticle("alpha");
		fParticleGun->SetParticleDefinition(particle);
	} else if ( mConfig.getConfig("ENVIRONMENT").getValue<std::string>("PARTICLE_NAME") == "gamma" ) {
		G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
		fParticleGun->SetParticleDefinition(particle);
	} else {
		std::cerr << "Invalid source particle." << std::endl;
		std::cerr << "Particle could be set in the configuration file with the key SOURCE_PARTICLE.in ENVIRONMENT" << std::endl;
		exit(1);
	}
	setEnergyDistribution(mConfig.getConfig("ENVIRONMENT").getValue<std::string>("PARTICLE_NAME"));
}

TPrimaryGeneratorAction::~TPrimaryGeneratorAction() {
	delete fParticleGun;
}

void TPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
	// Select a random energy based on intensity
	// 위의 CSV파일에서 정보를 들고와서 에너지에 따른 확률을 고려해서 만들어줌
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
	// Big Source(u, up)
	// position randomize

	std::string sourceType = mConfig.getConfig("ENVIRONMENT").hasKey("SOURCE_TYPE") ? mConfig.getConfig("ENVIRONMENT").getValue<std::string>("SOURCE_TYPE") : "U";

	if ( sourceType == "U" ) {
		G4double centerPosition[3] = {0, 0, 9.42}; // 6.62 + 1 + 1.8 
		G4double radius = 1.5 * mm;
		G4double r = radius * std::sqrt(G4UniformRand());
		G4double theta = 2 * M_PI * G4UniformRand();

		G4double randomX = centerPosition[0] + r * std::sin(theta);
		G4double randomY = centerPosition[1] + r * std::cos(theta);
		G4double randomZ = centerPosition[2];

		G4double cosTheta = 1 - 2 * G4UniformRand(); // Solid angle = 2pi
		G4double sinTheta = std::sqrt(1 - cosTheta * cosTheta);
		G4double phi_momentum = 2 * M_PI * G4UniformRand();
		G4double pX = sinTheta * std::sin(phi_momentum);
		G4double pY = sinTheta * std::cos(phi_momentum);
		G4double pZ = cosTheta;
		// 입자 위치 저장 
		fParticleGun->SetParticlePosition(G4ThreeVector(randomX, randomY, randomZ));
		// 입자 운동 방향 저장
		fParticleGun->SetParticleMomentumDirection(G4ThreeVector(pX, pY, pZ));
	} else if ( sourceType == "L" ) {
		G4double centerPosition[3] = {-5.9, 0, 0}; // 5.5 + 0.4
		G4double radius = 2.5 * mm;
		G4double r = radius * std::sqrt(G4UniformRand());
		G4double theta = 2 * M_PI * G4UniformRand();

		G4double randomX = centerPosition[0];
		G4double randomY = centerPosition[1] + r * std::sin(theta);
		G4double randomZ = centerPosition[2] + r * std::cos(theta);

		G4double cosTheta = 1 - 2 * G4UniformRand(); // Solid angle = 2pi
		G4double sinTheta = std::sqrt(1 - cosTheta * cosTheta);
		G4double phi_momentum = 2 * M_PI * G4UniformRand();
		G4double pX = -cosTheta;
		G4double pY = sinTheta * std::sin(phi_momentum);
		G4double pZ = sinTheta * std::cos(phi_momentum);
		// 입자 위치 저장 
		fParticleGun->SetParticlePosition(G4ThreeVector(randomX, randomY, randomZ));
		// 입자 운동 방향 저장
		fParticleGun->SetParticleMomentumDirection(G4ThreeVector(pX, pY, pZ));
	} else if ( sourceType == "R" ) {
		G4double centerPosition[3] = {5.9, 0, 0}; // 5.5 + 0.4
		G4double radius = 2.5 * mm;
		G4double r = radius * std::sqrt(G4UniformRand());
		G4double theta = 2 * M_PI * G4UniformRand();

		G4double randomX = centerPosition[0];
		G4double randomY = centerPosition[1] + r * std::sin(theta);
		G4double randomZ = centerPosition[2] + r * std::cos(theta);

		G4double cosTheta = 1 - 2 * G4UniformRand(); // Solid angle = 2pi
		G4double sinTheta = std::sqrt(1 - cosTheta * cosTheta);
		G4double phi_momentum = 2 * M_PI * G4UniformRand();
		G4double pX = cosTheta;
		G4double pY = sinTheta * std::sin(phi_momentum);
		G4double pZ = sinTheta * std::cos(phi_momentum);
		// 입자 위치 저장 
		fParticleGun->SetParticlePosition(G4ThreeVector(randomX, randomY, randomZ));
		// 입자 운동 방향 저장
		fParticleGun->SetParticleMomentumDirection(G4ThreeVector(pX, pY, pZ));
	}

	// 입자 생성
	fParticleGun->GeneratePrimaryVertex(anEvent);

}

const G4ParticleGun* TPrimaryGeneratorAction::GetParticleGun() const {
	return fParticleGun;
}

void TPrimaryGeneratorAction::setEnergyDistribution(std::string_view particle) {
	std::string csvPath;
	if ( particle == "alpha" ) {
		csvPath = amInfoPath;
	} else if ( particle == "gamma" ) {
		csvPath = gmInfoPath;
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
