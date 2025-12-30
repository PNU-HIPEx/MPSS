#include "TPlotManager.hpp"

#include "TFile.h"
#include "TH2.h"
#include "TH1.h"
#include "TKey.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"

#include "TPlot.hpp"
#include "config.hpp"

std::filesystem::path dataDir = DATA_DIR;

TPlotManager::TPlotManager(const KEI::TConfigFile& config) : mConfig(config) {
	gStyle->SetOptStat(0);

	mInputFilePath = dataDir / config.getConfig("FILE").getValue<std::string>("INPUT_FILE");
	mOutputPath = dataDir / config.getConfig("FILE").getValue<std::string>("OUTPUT_PATH");
	mPlotList = config.getConfigTitleSet();

	mInputFile = new TFile(static_cast<TString>(mInputFilePath), "READ");

	TList* list = mInputFile->GetListOfKeys();
	for ( int i = 0; i < list->GetSize(); i++ ) {
		TObject* obj = static_cast<TKey*>(list->At(i))->ReadObj();
		TString className = obj->ClassName();

		if ( std::find(mPlotList.begin(), mPlotList.end(), obj->GetName()) != mPlotList.end() ) {
			if ( className == "TH2D" ) {
				DrawTH2D(obj, mConfig.getConfig(obj->GetName()));
			} else if ( className == "TH1D" ) {
				DrawTH1D(obj, mConfig.getConfig(obj->GetName()));
			}
		}
	}
}

void TPlotManager::DrawTH2D(TObject* obj, const KEI::TConfig& config) {
	TH2D* hist = static_cast<TH2D*>(obj);
	TCanvas* canvas = KEI::TPlot::initCanvas(config);
	TLegend* legend = KEI::TPlot::initLegend(config);

	legend->SetMargin(0.1);
	legend->SetTextAlign(12);

	legend->AddEntry((TObject*)nullptr, Form("Entry: %.0f", hist->GetEffectiveEntries()), "");
	legend->AddEntry((TObject*)nullptr, Form("Mean X: %.3f", hist->GetMean(1)), "");
	legend->AddEntry((TObject*)nullptr, Form("Mean Y: %.3f", hist->GetMean(2)), "");
	legend->AddEntry((TObject*)nullptr, Form("StdDev X: %.3f", hist->GetStdDev(1)), "");
	legend->AddEntry((TObject*)nullptr, Form("StdDev Y: %.3f", hist->GetStdDev(2)), "");

	KEI::TPlot::drawPlot(canvas, hist, config);
	KEI::TPlot::saveLegend(canvas, legend);
	KEI::TPlot::saveCanvas(canvas, mOutputPath, config);
}

void TPlotManager::DrawTH1D(TObject* obj, const KEI::TConfig& config) {
	TH1D* hist = static_cast<TH1D*>(obj);
	TCanvas* canvas = KEI::TPlot::initCanvas(config);
	TLegend* legend = KEI::TPlot::initLegend(config);

	legend->SetMargin(0.1);
	legend->SetTextAlign(12);

	legend->AddEntry((TObject*)nullptr, Form("Entry: %.0f", hist->GetEffectiveEntries()), "");
	legend->AddEntry((TObject*)nullptr, Form("Mean X: %.3f", hist->GetMean()), "");
	legend->AddEntry((TObject*)nullptr, Form("StdDev X: %.3f", hist->GetStdDev()), "");

	if ( config.getTitle() == "INCIDENT_PARTICLE" ) {
		std::vector<TString> labels = {"#alpha", "#gamma", "e^{-}", "Other"};
		for ( size_t i = 0; i < labels.size(); i++ ) {
			hist->GetXaxis()->SetBinLabel(i + 1, labels[i]);
		}
		// hist->GetXaxis()->LabelsOption("");
	}

	KEI::TPlot::drawPlot(canvas, hist, config);
	KEI::TPlot::saveLegend(canvas, legend);
	KEI::TPlot::saveCanvas(canvas, mOutputPath, config);
}