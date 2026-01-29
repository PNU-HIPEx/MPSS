#include "TAnalysisManager.hpp"
#include "TTree.h"
#include "TROOT.h"
#include "cpptqdm.h"
#include "TPlot.hpp"

#include "config.hpp"
#include <iostream>

const std::filesystem::path dataDir = DATA_DIR;

TAnalysisManager::TAnalysisManager(const KEI::TConfigFile& config) : mConfig(config) {
	mInputFilePath = dataDir / mConfig.getConfig("FILE").getValue<std::string>("INPUT_FILE");
	mPlotList = config.getConfigTitleSet();
	std::cout << "Looking for input files with pattern: " << mInputFilePath << std::endl;

	if ( std::filesystem::exists(mInputFilePath.parent_path() / (std::string(mInputFilePath.stem()) + "_t0.root")) ) {
		int iFile = 0;
		while ( true ) {
			std::filesystem::path tempPath = mInputFilePath.parent_path() / (std::string(mInputFilePath.stem()) + "_t" + std::to_string(iFile) + ".root");
			if ( std::filesystem::exists(tempPath) ) {
				std::cout << "Found file: " << tempPath << std::endl;
				TFile* file = new TFile(static_cast<TString>(tempPath.string()), "READ");
				if ( file && !file->IsZombie() ) {
					mInputFileList.push_back(file);
				} else {
					std::cerr << "Error opening file: " << tempPath << std::endl;
					delete file;
				}
				iFile++;
			} else {
				break;
			}
		}
		std::cout << "Total files loaded: " << mInputFileList.size() << std::endl;
	} else {
		std::cout << "No multithreaded files found, trying single file..." << std::endl;
		if ( std::filesystem::exists(mInputFilePath) ) {
			TFile* file = new TFile(static_cast<TString>(mInputFilePath.string()), "READ");
			if ( file && !file->IsZombie() ) {
				mInputFileList.push_back(file);
				std::cout << "Loaded single file: " << mInputFilePath << std::endl;
			}
		}
	}

	if ( mInputFileList.empty() ) {
		std::cerr << "Error: No input files found!" << std::endl;
	}
}

TAnalysisManager::~TAnalysisManager() {
	// 히스토그램 메모리 정리
	for ( auto& histPair : mHistogram1D ) {
		delete histPair.second;
	}
	mHistogram1D.clear();

	for ( auto& histPair : mHistogram2D ) {
		delete histPair.second;
	}

	// 파일 정리
	for ( auto& file : mInputFileList ) {
		if ( file ) {
			file->Close();
			delete file;
		}
	}
	mInputFileList.clear();
}

void TAnalysisManager::initHistograms() {
	for ( const std::string& plot : mPlotList ) {
		if ( mConfig.hasConfig(plot) && plot != "FILE" ) {
			if ( mConfig.getConfig(plot).getValue<std::string>("TYPE") == "2H" ) {
				TH2* hist = KEI::TPlot::init2DHist(mConfig.getConfig(plot));
				mHistogram2D.insert({plot, hist});
			} else if ( mConfig.getConfig(plot).getValue<std::string>("TYPE") == "1H" ) {
				TH1* hist = KEI::TPlot::init1DHist(mConfig.getConfig(plot));
				mHistogram1D.insert({plot, hist});
			}
		}

	}
}

void TAnalysisManager::extractData() {
	for ( auto& inputFile : mInputFileList ) {
		TTree* tree = static_cast<TTree*>(inputFile->Get("incident"));

		if ( !tree ) {
			std::cerr << "Error: Could not find 'incident' tree in file" << std::endl;
			continue;
		}

		Int_t eventID, trackID, parentID, particleID;
		tree->SetBranchAddress("EventID", &eventID);
		tree->SetBranchAddress("TrackID", &trackID);
		tree->SetBranchAddress("ParentID", &parentID);
		tree->SetBranchAddress("ParticleID", &particleID);

		Double_t incidentX, incidentY, incidentZ;
		tree->SetBranchAddress("Incident_position_X", &incidentX);
		tree->SetBranchAddress("Incident_position_Y", &incidentY);
		tree->SetBranchAddress("Incident_position_Z", &incidentZ);
		Double_t depositX = 0., depositY = 0.;
		tree->SetBranchAddress("Deposit_mean_X", &depositX);
		tree->SetBranchAddress("Deposit_mean_Y", &depositY);

		Double_t energyDeposit = 0.;
		tree->SetBranchAddress("Deposited_energy", &energyDeposit);
		std::array<Double_t, 2> sliceRange = mConfig.getConfig("DEPOSIT_POSITION_SLICE").getValue<Double_t, 2>("RANGE");

		Long64_t nEntries = tree->GetEntries();
		std::cout << "Processing " << nEntries << " entries..." << std::endl;
		ProgressBar::createGlobal(nEntries);
		for ( Long64_t iEntry = 0; iEntry < nEntries; ++iEntry ) {
			ProgressBar::incrementGlobal();
			tree->GetEntry(iEntry);
			if ( isDraw("INCIDENT_POSITION_XY") ) mHistogram2D["INCIDENT_POSITION_XY"]->Fill(incidentX, incidentY);
			if ( isDraw("EVENT_ID") ) mHistogram1D["EVENT_ID"]->Fill(eventID);
			if ( isDraw("TRACK_ID") ) mHistogram1D["TRACK_ID"]->Fill(trackID);
			if ( isDraw("PARENT_ID") ) mHistogram1D["PARENT_ID"]->Fill(parentID);
			if ( isDraw("INCIDENT_PARTICLE") ) {
				if ( particleID == 1000020040 ) {
					mHistogram1D["INCIDENT_PARTICLE"]->Fill(0);
				} else if ( particleID == 11 ) {
					mHistogram1D["INCIDENT_PARTICLE"]->Fill(1);
				} else if ( particleID == 22 ) {
					mHistogram1D["INCIDENT_PARTICLE"]->Fill(1);
				} else {
					std::cout << particleID << std::endl;
				}
			}
			if ( energyDeposit > 0 ) {
				if ( isDraw("INCIDENT_POSITION_XY_DEPOSIT") ) mHistogram2D["INCIDENT_POSITION_XY_DEPOSIT"]->Fill(incidentX, incidentY);
				if ( isDraw("INCIDENT_POSITION_Z_DEPOSIT") ) mHistogram1D["INCIDENT_POSITION_Z_DEPOSIT"]->Fill(incidentZ);
				if ( isDraw("ENERGY_DEPOSIT") ) mHistogram1D["ENERGY_DEPOSIT"]->Fill(energyDeposit);
				if ( isDraw("DEPOSIT_POSITION_XY") ) mHistogram2D["DEPOSIT_POSITION_XY"]->Fill(depositX, depositY);
				if ( isDraw("DEPOSIT_POSITION_SLICE") && sliceRange[0] * 0.027 < depositY && depositY < sliceRange[1] * 0.027 ) {
					mHistogram1D["DEPOSIT_POSITION_SLICE"]->Fill(depositX);
				}

			}
		}
		ProgressBar::destroyGlobal();

		std::cout << "Processed " << nEntries << " entries from file" << std::endl;
	}
}
bool TAnalysisManager::isDraw(std::string_view plotName) {
	if ( std::find(mPlotList.begin(), mPlotList.end(), plotName) != mPlotList.end() ) {
		return true;
	} else {
		return false;
	}
}

void TAnalysisManager::savePlots() {
	std::filesystem::path temp = dataDir / mConfig.getConfig("FILE").getValue<std::string>("OUTPUT_FILE");
	std::filesystem::path outputFilePath = temp;

	// 절대 경로로 변환
	std::filesystem::path absolutePath = std::filesystem::absolute(outputFilePath);
	std::filesystem::create_directories(absolutePath.parent_path());

	TFile* outputFile = new TFile(static_cast<TString>(absolutePath.string()), "RECREATE");

	if ( !outputFile || outputFile->IsZombie() ) {
		std::cerr << "Error: Could not create output file: " << absolutePath << std::endl;
		return;
	}

	std::cout << "Saving histograms to: " << absolutePath << std::endl;

	for ( const auto& histPair : mHistogram1D ) {  // const auto& 사용
		if ( histPair.second ) {  // null 포인터 확인
			histPair.second->Write();
		} else {
			std::cerr << "Warning: Null histogram for " << histPair.first << std::endl;
		}
	}

	for ( const auto& histPair : mHistogram2D ) {  // const auto& 사용
		if ( histPair.second ) {  // null 포인터 확인
			histPair.second->Write();
		} else {
			std::cerr << "Warning: Null histogram for " << histPair.first << std::endl;
		}
	}

	outputFile->Close();
	delete outputFile;
	std::cout << "Analysis completed successfully!" << std::endl;
}