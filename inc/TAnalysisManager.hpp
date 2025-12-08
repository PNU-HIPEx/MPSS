#ifndef __TANALYSISMANAGER__
#define __TANALYSISMANAGER__

#include<filesystem>
#include<vector>
#include<string>
#include<unordered_map>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

#include "TConfig.hpp"

class TAnalysisManager {
	public:
		TAnalysisManager(const KEI::TConfigFile& config);
		~TAnalysisManager();

		void initHistograms();
		void extractData();
		void savePlots();
	private:
		KEI::TConfigFile mConfig;
		std::filesystem::path mInputFilePath;
		std::vector<TFile*> mInputFileList;

		std::unordered_map<std::string, TH1D*> mHistogram1D; 
		std::unordered_map<std::string, TH2D*> mHistogram2D;
};

#endif