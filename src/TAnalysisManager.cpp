#include "TAnalysisManager.hpp"
#include "TTree.h"
#include "TROOT.h"
#include "cpptqdm.h"
#include "TPlot.hpp"
#include "TRotation.h"
#include "TVector3.h"
#include "config.hpp"
#include <iostream>

const std::filesystem::path dataDir = DATA_DIR;

TAnalysisManager::TAnalysisManager(const KEI::TConfigFile& config) : mConfig(config) {
	mInputFilePath = dataDir / mConfig.getConfig("FILE").getValue<std::string>("INPUT_FILE");
	mPlotList = config.getConfigTitleSet();

	int nRun = mConfig.getConfig("FILE").hasKey("RUN_NUM") ? mConfig.getConfig("FILE").getValue<int>("RUN_NUM") : 1;
	int nThread = mConfig.getConfig("FILE").hasKey("THREAD_NUM") ? mConfig.getConfig("FILE").getValue<int>("THREAD_NUM") : 0;
	if ( nThread != 0 ) {
		for ( int iRun = 0; iRun < nRun; iRun++ ) {
			for ( int iThread = 0; iThread < nThread; iThread++ ) {
				std::filesystem::path path = mInputFilePath.parent_path() / (std::string(mInputFilePath.stem()) + "_Run" + std::to_string(iRun) + "_t" + std::to_string(iThread) + ".root");
				TFile* file = new TFile(static_cast<TString>(path.string()), "READ");
				mInputFileList.push_back(file);
			}
		}
	} else {
		for ( int iRun = 0; iRun < nRun; iRun++ ) {
			std::filesystem::path path = mInputFilePath.parent_path() / (std::string(mInputFilePath.stem()) + "_Run" + std::to_string(iRun) + ".root");
			TFile* file = new TFile(static_cast<TString>(path.string()), "READ");
			mInputFileList.push_back(file);
		}
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
			} else if ( mConfig.getConfig(plot).getValue<std::string>("TYPE") == "3H" ) {
				TH3* hist = KEI::TPlot::init3DHist(mConfig.getConfig(plot));
				mHistogram3D.insert({plot, hist});
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
		std::array<Double_t, 2> sliceRangeY = mConfig.getConfig("DEPOSIT_POSITION_SLICE_Y").getValue<Double_t, 2>("RANGE");

		Long64_t nEntries = tree->GetEntries();
		ProgressBar::createGlobal(nEntries);
		for ( Long64_t iEntry = 0; iEntry < nEntries; ++iEntry ) {
			ProgressBar::incrementGlobal();
			tree->GetEntry(iEntry);

			std::array<double, 3> detectorRotation = mConfig.getConfig("INCIDENT_POSITION_XYZ").hasKey("DETECTOR_ROTATION") ? mConfig.getConfig("INCIDENT_POSITION_XYZ").getValue<double, 3>("DETECTOR_ROTATION") : std::array<double, 3>{0., 0., 0.};
			std::array<double, 3> detectorPosition = mConfig.getConfig("INCIDENT_POSITION_XYZ").hasKey("DETECTOR_POSITION") ? mConfig.getConfig("INCIDENT_POSITION_XYZ").getValue<double, 3>("DETECTOR_POSITION") : std::array<double, 3>{0., 0., 0.};
			TRotation rot;
			rot.RotateX(-detectorRotation[0] * 3.141592 / 180.);
			rot.RotateY(-detectorRotation[1] * 3.141592 / 180.);
			rot.RotateZ(-detectorRotation[2] * 3.141592 / 180.);
			TVector3 incidentPosition(incidentX - detectorPosition[0], incidentY - detectorPosition[1], incidentZ - detectorPosition[2]);
			incidentPosition = rot * incidentPosition;

			if ( isDraw("INCIDENT_POSITION_XY") ) mHistogram2D["INCIDENT_POSITION_XY"]->Fill(incidentPosition[0], incidentPosition[1]);
			if ( isDraw("INCIDENT_POSITION_XYZ") ) mHistogram3D["INCIDENT_POSITION_XYZ"]->Fill(incidentPosition[0], incidentPosition[1], incidentPosition[2]);
			if ( isDraw("INCIDENT_POSITION_Z") ) mHistogram1D["INCIDENT_POSITION_Z"]->Fill(incidentZ);
			if ( isDraw("EVENT_ID") ) mHistogram1D["EVENT_ID"]->Fill(eventID);
			if ( isDraw("TRACK_ID") ) mHistogram1D["TRACK_ID"]->Fill(trackID);
			if ( isDraw("PARENT_ID") ) mHistogram1D["PARENT_ID"]->Fill(parentID);
			if ( isDraw("INCIDENT_PARTICLE") ) {
				if ( particleID == 1000020040 ) { // Alpha particle
					mHistogram1D["INCIDENT_PARTICLE"]->Fill(0);
				} else if ( particleID == 11 ) {
					mHistogram1D["INCIDENT_PARTICLE"]->Fill(1);
				} else if ( particleID == 22 ) {
					mHistogram1D["INCIDENT_PARTICLE"]->Fill(2);
				} else {
					std::cout << particleID << std::endl;
				}
			}
			if ( energyDeposit > 0 ) {
				if ( isDraw("INCIDENT_POSITION_XY_DEPOSIT") ) mHistogram2D["INCIDENT_POSITION_XY_DEPOSIT"]->Fill(incidentX, incidentY);
				if ( isDraw("INCIDENT_POSITION_Z_DEPOSIT") ) mHistogram1D["INCIDENT_POSITION_Z_DEPOSIT"]->Fill(incidentZ);
				if ( isDraw("ENERGY_DEPOSIT") ) mHistogram1D["ENERGY_DEPOSIT"]->Fill(energyDeposit);
				if ( isDraw("DEPOSIT_POSITION_XY") ) mHistogram2D["DEPOSIT_POSITION_XY"]->Fill(depositX, depositY);
				if ( isDraw("DEPOSIT_POSITION_SLICE") && sliceRange[0] * 0.027 < incidentPosition[1] && incidentPosition[1] < sliceRange[1] * 0.027 ) {
					mHistogram1D["DEPOSIT_POSITION_SLICE"]->Fill(incidentPosition[0]);
				}
				if ( isDraw("DEPOSIT_POSITION_SLICE_Y") && sliceRangeY[0] * 0.029 < incidentPosition[0] && incidentPosition[0] < sliceRangeY[1] * 0.029 ) {
					mHistogram1D["DEPOSIT_POSITION_SLICE_Y"]->Fill(incidentPosition[1]);
				}

			}
		}
		ProgressBar::destroyGlobal();

		std::cout << "Processed " << nEntries << " entries from file \"" << inputFile->GetName() << "\"" << std::endl;
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

	for ( const auto& histPair : mHistogram3D ) {  // const auto& 사용
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