#include<filesystem>

#include "TRunAction.hpp"
#include "G4Run.hh"
#include "TAnalysisManager.hpp"
#include "config.hpp"

TRunAction::TRunAction(const KEI::TConfigFile& config) : G4UserRunAction(), mConfig(config) {
	std::vector<G4double> positionVector;
	std::vector<G4double> momentumVector;
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	analysisManager->CreateH3("P_INI_POS_XYZ", "Initial Position of Primary Particles", 100, -25, 25, 100, -25, 25, 100, -17.5, 17.5, "mm", "mm", "mm");

	analysisManager->CreateH2("P_INI_ANG_TP", "Initial Angle of Primary Particles", 180, 0, 180, 360, -180, 180);

	analysisManager->CreateH1("P_INI_KE", "Initial Kinetic Energy of Primary Particles", 6000, 0, 6000, "keV");

	analysisManager->CreateH3("S_INI_POS_XYZ", "Initial Position of Secondary Particles", 100, -5, 5, 100, -5, 5, 100, -5, 5, "mm", "mm", "mm");
	analysisManager->CreateH2("S_INI_ANG_TP", "Initial Angle of Secondary Particles", 180, 0, 180, 360, -180, 180);
	analysisManager->CreateH1("S_INI_KE", "Initial Kinetic Energy of Secondary Particles", 6000, 0, 6000, "keV");

	// Secondary Particle 들의 종류와 개수를 추적하는 코드 작성해야함

	analysisManager->CreateNtuple("incident", "Incident Particle Information");
	analysisManager->CreateNtupleIColumn("EventID");
	analysisManager->CreateNtupleIColumn("TrackID");
	analysisManager->CreateNtupleIColumn("ParentID");
	analysisManager->CreateNtupleIColumn("ParticleID");

	analysisManager->CreateNtupleDColumn("Vertex_position_X");
	analysisManager->CreateNtupleDColumn("Vertex_position_Y");
	analysisManager->CreateNtupleDColumn("Vertex_position_Z");
	analysisManager->CreateNtupleDColumn("Vertex_momentum_PX");
	analysisManager->CreateNtupleDColumn("Vertex_momentum_PY");
	analysisManager->CreateNtupleDColumn("Vertex_momentum_PZ");
	analysisManager->CreateNtupleDColumn("Vertex_kinetic_energy");
	analysisManager->CreateNtupleIColumn("Vertex_volume");

	analysisManager->CreateNtupleDColumn("Incident_position_X");
	analysisManager->CreateNtupleDColumn("Incident_position_Y");
	analysisManager->CreateNtupleDColumn("Incident_position_Z");
	analysisManager->CreateNtupleDColumn("Incident_momentum_PX");
	analysisManager->CreateNtupleDColumn("Incident_momentum_PY");
	analysisManager->CreateNtupleDColumn("Incident_momentum_PZ");
	analysisManager->CreateNtupleDColumn("Incident_kinetic_energy");
	analysisManager->CreateNtupleDColumn("Deposited_energy");
	analysisManager->CreateNtupleIColumn("Step_in_detector");
	analysisManager->CreateNtupleDColumn("Deposit_mean_X");
	analysisManager->CreateNtupleDColumn("Deposit_mean_Y");
	analysisManager->CreateNtupleDColumn("Deposit_mean_Z");
	analysisManager->CreateNtupleDColumn("Deposit_stddev_X");
	analysisManager->CreateNtupleDColumn("Deposit_stddev_Y");
	analysisManager->CreateNtupleDColumn("Deposit_stddev_Z");

	analysisManager->CreateNtupleDColumn("Final_position_X");
	analysisManager->CreateNtupleDColumn("Final_position_Y");
	analysisManager->CreateNtupleDColumn("Final_position_Z");
	analysisManager->CreateNtupleIColumn("Final_volume");
	analysisManager->FinishNtuple(0);
}

void TRunAction::BeginOfRunAction(const G4Run* run) {
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	std::filesystem::path dataDir = DATA_DIR;
	std::filesystem::path temp = dataDir / mConfig.getConfig("FILE").getValue<std::string>("OUTPUT_FILE");
	std::filesystem::path outputFilePath = temp.parent_path() / (temp.stem().string() + "_Run" + std::to_string(run->GetRunID()) + ".root");
	// 절대 경로로 변환
	std::filesystem::path absolutePath = std::filesystem::absolute(outputFilePath);
	std::filesystem::create_directories(absolutePath.parent_path());

	analysisManager->OpenFile(absolutePath.string());

	int totalEvents = run->GetNumberOfEventToBeProcessed();
	ProgressBar::createGlobal(totalEvents);
}

void TRunAction::EndOfRunAction(const G4Run* run) {
	ProgressBar::destroyGlobal();
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->Write();
	analysisManager->CloseFile();
}