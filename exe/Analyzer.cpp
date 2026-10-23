#include "TAnalysisManager.hpp"

#include "TConfig.hpp"

#include "config.hpp"

std::filesystem::path binaryDir = BINARY_DIR;
std::filesystem::path configPath = binaryDir / "config/analyzer.conf";

int main(int argc, char** argv) {
	KEI::TConfigFile config(configPath);
	TAnalysisManager analysisManager(config);
	analysisManager.initHistograms();
	analysisManager.extractData();
	analysisManager.savePlots();

	return 0;
}