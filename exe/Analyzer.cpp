#include "TAnalysisManager.hpp"

#include "TConfig.hpp"

int main(int argc, char** argv) {
	KEI::TConfigFile config("../config/analyzer.conf");
	TAnalysisManager analysisManager(config);
	analysisManager.initHistograms();
	analysisManager.extractData();
	analysisManager.savePlots();

	
	return 0;
}