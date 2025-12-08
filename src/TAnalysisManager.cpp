#include "TAnalysisManager.hpp"
#include "TTree.h"
#include "TROOT.h"
#include "cpptqdm.h"

TAnalysisManager::TAnalysisManager(const KEI::TConfigFile& config) : mConfig(config) {
    mInputFilePath = mConfig.getConfig("FILE").getValue<std::string>("INPUT_FILE");
    
    std::cout << "Looking for input files with pattern: " << mInputFilePath << std::endl;
    
    if ( std::filesystem::exists(mInputFilePath.parent_path() / (std::string(mInputFilePath.stem()) + "_t0.root")) ) {
        int iFile = 0;
        while(true) {
            std::filesystem::path tempPath = mInputFilePath.parent_path() / (std::string(mInputFilePath.stem()) + "_t" + std::to_string(iFile) + ".root");
            if ( std::filesystem::exists(tempPath) ) {
                std::cout << "Found file: " << tempPath << std::endl;
                TFile* file = new TFile(static_cast<TString>(tempPath.string()), "READ");
                if (file && !file->IsZombie()) {
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
        if (std::filesystem::exists(mInputFilePath)) {
            TFile* file = new TFile(static_cast<TString>(mInputFilePath.string()), "READ");
            if (file && !file->IsZombie()) {
                mInputFileList.push_back(file);
                std::cout << "Loaded single file: " << mInputFilePath << std::endl;
            }
        }
    }
    
    if (mInputFileList.empty()) {
        std::cerr << "Error: No input files found!" << std::endl;
    }
}

TAnalysisManager::~TAnalysisManager() {
    // 히스토그램 메모리 정리
    for (auto& histPair : mHistogram1D) {
        delete histPair.second;
    }
    mHistogram1D.clear();

	for ( auto& histPair : mHistogram2D) {
		delete histPair.second;
	}
    
    // 파일 정리
    for (auto& file : mInputFileList) {
        if (file) {
            file->Close();
            delete file;
        }
    }
    mInputFileList.clear();
}

void TAnalysisManager::initHistograms() {
	TH2D* hist1 = new TH2D("incidentPositionZX", "", 1024, -30, 30, 512, -15, 15);
	mHistogram2D.insert({"IncidentPositionZX", hist1});

}

void TAnalysisManager::extractData() {
    for ( auto& inputFile : mInputFileList ) {
        TTree* tree = static_cast<TTree*>(inputFile->Get("incident"));
        
        if (!tree) {
            std::cerr << "Error: Could not find 'incident' tree in file" << std::endl;
            continue;
        }
        
        Int_t eventID, trackID, parentID, particleID;
        tree->SetBranchAddress("EventID", &eventID);
        tree->SetBranchAddress("TrackID", &trackID);
        tree->SetBranchAddress("ParentID", &parentID);
        tree->SetBranchAddress("ParticleID", &particleID);
        
        Double_t initPosX, initPosY, initPosZ;
        tree->SetBranchAddress("Initial_position_X", &initPosX);
        tree->SetBranchAddress("Initial_position_Y", &initPosY);
        tree->SetBranchAddress("Initial_position_Z", &initPosZ);
        
        Double_t initMomX, initMomY, initMomZ;
        tree->SetBranchAddress("Initial_momentum_PX", &initMomX);
        tree->SetBranchAddress("Initial_momentum_PY", &initMomY);
        tree->SetBranchAddress("Initial_momentum_PZ", &initMomZ);

		Double_t initKineticEnergy;
		tree->SetBranchAddress("Initial_kinetic_energy", &initKineticEnergy);
        
        Long64_t nEntries = tree->GetEntries();
        std::cout << "Processing " << nEntries << " entries..." << std::endl;
        ProgressBar::createGlobal(nEntries);
        for ( Long64_t iEntry = 0; iEntry < nEntries; ++iEntry ) {
			ProgressBar::incrementGlobal();
            tree->GetEntry(iEntry);
			mHistogram2D["IncidentPositionZX"]->Fill(initPosX, initPosZ);
        }
		ProgressBar::destroyGlobal();
        
        std::cout << "Processed " << nEntries << " entries from file" << std::endl;
    }
}

void TAnalysisManager::savePlots() {
    std::filesystem::path temp = mConfig.getConfig("FILE").getValue<std::string>("OUTPUT_FILE");
    std::filesystem::path outputFilePath = temp;
    
    // 절대 경로로 변환
    std::filesystem::path absolutePath = std::filesystem::absolute(outputFilePath);
    std::filesystem::create_directories(absolutePath.parent_path());

    TFile* outputFile = new TFile(static_cast<TString>(absolutePath.string()), "RECREATE");
    
    if (!outputFile || outputFile->IsZombie()) {
        std::cerr << "Error: Could not create output file: " << absolutePath << std::endl;
        return;
    }
    
    std::cout << "Saving histograms to: " << absolutePath << std::endl;
    
    for ( const auto& histPair : mHistogram1D ) {  // const auto& 사용
        if (histPair.second) {  // null 포인터 확인
            histPair.second->Write();
        } else {
            std::cerr << "Warning: Null histogram for " << histPair.first << std::endl;
        }
    }

	for ( const auto& histPair : mHistogram2D ) {  // const auto& 사용
        if (histPair.second) {  // null 포인터 확인
            histPair.second->Write();
        } else {
            std::cerr << "Warning: Null histogram for " << histPair.first << std::endl;
        }
    }
    
    outputFile->Close();
    delete outputFile;
    std::cout << "Analysis completed successfully!" << std::endl;
}