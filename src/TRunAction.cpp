#include<filesystem>

#include "TRunAction.hpp"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "TAnalysisManager.hpp"

TRunAction::TRunAction(const KEI::TConfigFile& config) : G4UserRunAction(), mConfig(config) {
	std::vector<G4double> positionVector;
	std::vector<G4double> momentumVector;
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->CreateNtuple("hits", "Hit Information");
	analysisManager->CreateNtupleIColumn("EventID");
	analysisManager->CreateNtupleIColumn("TrackID");
	analysisManager->CreateNtupleIColumn("ParentID");
	analysisManager->CreateNtupleIColumn("ParticleID");
	analysisManager->CreateNtupleDColumn("Initial position X");
	analysisManager->CreateNtupleDColumn("Initial position Y");
	analysisManager->CreateNtupleDColumn("Initial position Z");
	analysisManager->CreateNtupleDColumn("Initial momentum PX");
	analysisManager->CreateNtupleDColumn("Initial momentum PY");
	analysisManager->CreateNtupleDColumn("Initial momentum PZ");
	analysisManager->CreateNtupleDColumn("Final position X");
	analysisManager->CreateNtupleDColumn("Final position Y");
	analysisManager->CreateNtupleDColumn("Final position Z");
	analysisManager->CreateNtupleDColumn("Final momentum PX");
	analysisManager->CreateNtupleDColumn("Final momentum PY");
	analysisManager->CreateNtupleDColumn("Final momentum PZ");
	analysisManager->FinishNtuple();

	analysisManager->CreateH2("Initial Position XZ", "Initial Position", 100, -5, 5, 100, -5, 5, "mm", "mm", "X", "Z");
	analysisManager->CreateH1("Initial Position Y", "Initial Position Y", 100, 5, 15);
	analysisManager->CreateNtuple("incident", "Incident Particle Information");
	analysisManager->CreateNtupleIColumn(1, "EventID");
	analysisManager->CreateNtupleIColumn(1, "TrackID");
	analysisManager->CreateNtupleIColumn(1, "ParentID");
	analysisManager->CreateNtupleIColumn(1, "ParticleID");
	analysisManager->CreateNtupleDColumn(1, "Incident position X");
	analysisManager->CreateNtupleDColumn(1, "Incident position Y");
	analysisManager->CreateNtupleDColumn(1, "Incident position Z");
	analysisManager->CreateNtupleDColumn(1, "Incident momentum PX");
	analysisManager->CreateNtupleDColumn(1, "Incident momentum PY");
	analysisManager->CreateNtupleDColumn(1, "Incident momentum PZ");
	analysisManager->FinishNtuple(1);
}

void TRunAction::BeginOfRunAction(const G4Run* run) {
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	std::filesystem::path temp = mConfig.getConfig("FILE").getValue<std::string>("OUTPUT_FILE");
	std::filesystem::path outputFilePath = temp.parent_path() / (temp.stem().string() + "_" + std::to_string(run->GetRunID()) + ".root");
	
	// 절대 경로로 변환
	std::filesystem::path absolutePath = std::filesystem::absolute(outputFilePath);
	std::filesystem::create_directories(absolutePath.parent_path());
	
	analysisManager->OpenFile(absolutePath.string());
}

void TRunAction::EndOfRunAction(const G4Run* run) {
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->Write();
	analysisManager->CloseFile();
}

