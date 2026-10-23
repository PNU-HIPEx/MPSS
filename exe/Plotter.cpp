#include<filesystem>

#include "TPlotManager.hpp"

#include "TConfig.hpp"

#include "config.hpp"

std::filesystem::path binaryDir = BINARY_DIR;
std::filesystem::path configPath = binaryDir / "config/plotter.conf";

int main() {
	KEI::TConfigFile config(configPath);
	TPlotManager plotManager(config);
	return 0;
}