#include "TPlotManager.hpp"

#include "TConfig.hpp"

int main() {
	KEI::TConfigFile config("../config/plotter.conf");
	TPlotManager plotManager(config);
	return 0;
}