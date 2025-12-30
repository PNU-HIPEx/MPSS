#ifndef __TPLOTMANAGER__
#define __TPLOTMANAGER__

#include "TConfig.hpp"

class TFile;
class TObject;

class TPlotManager {
public:
	TPlotManager(const KEI::TConfigFile& config);
private:
	KEI::TConfigFile mConfig;
	std::vector<std::string> mPlotList;

	std::filesystem::path mInputFilePath;
	std::filesystem::path mOutputPath;

	TFile* mInputFile;
public:
	void DrawTH2D(TObject* obj, const KEI::TConfig& config);
	void DrawTH1D(TObject* obj, const KEI::TConfig& config);

};

#endif