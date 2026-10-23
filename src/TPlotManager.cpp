#include "TPlotManager.hpp"

#include "TFile.h"
#include "TH2.h"
#include "TH1.h"
#include "TH3.h"
#include "TKey.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TF1.h"

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

	TH2* depositPosition;
	for ( int i = 0; i < list->GetSize(); i++ ) {
		TObject* obj = static_cast<TKey*>(list->At(i))->ReadObj();
		TString className = obj->ClassName();

		if ( std::find(mPlotList.begin(), mPlotList.end(), obj->GetName()) != mPlotList.end() ) {
			if ( className == "TH2D" ) {
				if (obj->GetName() == static_cast<TString>("DEPOSIT_POSITION_XY")) {
					depositPosition = static_cast<TH2*>(obj->Clone());
				}
				DrawTH2D(obj, mConfig.getConfig(obj->GetName()));
			} else if ( className == "TH1D" ) {
				DrawTH1D(obj, mConfig.getConfig(obj->GetName()));
			} else if ( className == "TH3D" ) {
				DrawTH3D(obj, mConfig.getConfig(obj->GetName()));
			}
		}
	}

	if ( mConfig.hasConfig("SLICE_X_COLLECTION") && mConfig.hasConfig("DEPOSIT_POSITION_XY")) {
		TCanvas* canvas = KEI::TPlot::initCanvas(mConfig.getConfig("SLICE_X_COLLECTION"));
		TLegend* legend = KEI::TPlot::initLegend(mConfig.getConfig("SLICE_X_COLLECTION"));
		KEI::TConfig collectionConfig = mConfig.getConfig("SLICE_X_COLLECTION");
		int nPlot = 0;
		while(true) {
			std::string configName = "P" + std::to_string(nPlot);
			if ( !collectionConfig.hasConfig(configName)) {
				break;
			} else {
				nPlot++;
			}
		}

		for ( int i = 0; i < nPlot; i++) {
			TH1D* collectionPlot = new TH1D(static_cast<TString>("collectionPlot"+ std::to_string(i)), "", 1024, 0, 1024);
			std::array<int, 2> range = collectionConfig.getConfig("P"+std::to_string(i)).getValue<int,2>("REGION");
			for ( int y = range[0]; y < range[1]; y++) {
				for ( int x = 0; x < 1024; x++) {
					collectionPlot->Fill(x, depositPosition->GetBinContent(x,y));
				}
			}
			legend->AddEntry(collectionPlot, Form("%d < y < %d", range[0], range[1]), "L");
			KEI::TPlot::drawPlot(canvas, collectionPlot, collectionConfig.getConfig("P"+std::to_string(i)));
		}
		KEI::TPlot::saveLegend(canvas, legend);
		KEI::TPlot::saveCanvas(canvas, mOutputPath, collectionConfig);
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

	if ( config.getTitle() == "INCIDENT_PARTICLE" || config.getTitle() == "DEPOSIT_PARTICLE" ) {
		std::vector<TString> labels = {"#alpha", "e^{-}", "#gamma", "Other"};
		for ( size_t i = 0; i < labels.size(); i++ ) {
			hist->GetXaxis()->SetBinLabel(i + 1, labels[i]);
		}
		// hist->GetXaxis()->LabelsOption("");
	}
	KEI::TPlot::drawPlot(canvas, hist, config);
	if ( config.getTitle() == "DEPOSIT_POSITION_SLICE" ) {
		// TF1* func = new TF1("func", "[0]*x+[1]", -14, 3);
		// TF1* func = new TF1("f1", "[0] * exp( - pow((x - [1])/[2],2 * [3]))+ [4]", -4, 4);
		// func->SetParameters(hist->GetMaximum(), 0, 2.6, 2, 100);
		// hist->Fit(func, "R");
		// func->Draw("SAME");
	}
	KEI::TPlot::saveLegend(canvas, legend);
	KEI::TPlot::saveCanvas(canvas, mOutputPath, config);
}

void TPlotManager::DrawTH3D(TObject* obj, const KEI::TConfig& config) {
	TH3D* hist = static_cast<TH3D*>(obj);
	TCanvas* canvas = KEI::TPlot::initCanvas(config);
	TLegend* legend = KEI::TPlot::initLegend(config);

	legend->SetMargin(0.1);
	legend->SetTextAlign(12);

	legend->AddEntry((TObject*)nullptr, Form("Entry: %.0f", hist->GetEffectiveEntries()), "");
	legend->AddEntry((TObject*)nullptr, Form("Mean X: %.3f", hist->GetMean()), "");
	legend->AddEntry((TObject*)nullptr, Form("StdDev X: %.3f", hist->GetStdDev()), "");

	KEI::TPlot::drawPlot(canvas, hist, config);

	KEI::TPlot::saveLegend(canvas, legend);
	KEI::TPlot::saveCanvas(canvas, mOutputPath, config);
}