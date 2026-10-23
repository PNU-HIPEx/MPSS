#include<filesystem>
#include "TH2.h"
#include "TCanvas.h"
#include "TDataPlotter.hpp"

int main() {
	std::filesystem::path lrPath = "/mnt/homes/ychoi/MPS/EXPERIMENT/PLA/ANALYSIS/LR_analysis.root";
	TFile* lrFile = new TFile(static_cast<TString>(lrPath));
	TH2* lrMap = static_cast<TH2D*>(lrFile->Get(""))

	std::filesystem::path rPath = "/mnt/homes/ychoi/MPS/EXPERIMENT/PLA/ANALYSIS/R_analysis.root";
	TDataPlotter rPlot(rPath);
	rPlot.initPlot();
	rPlot.fillPlot();
	TH2* rClustermap = rPlot.getClustermap();

	std::filesystem::path lPath = "/mnt/homes/ychoi/MPS/EXPERIMENT/PLA/ANALYSIS/L_analysis.root";
	TDataPlotter lPlot(lPath);
	lPlot.initPlot();
	lPlot.fillPlot();
	TH2* lClustermap = lPlot.getClustermap();

	std::filesystem::path gammaPath = "/mnt/homes/ychoi/MPS/EXPERIMENT/PLA/ANALYSIS/NON.root";
	TDataPlotter gammaPlot(gammaPath);
	gammaPlot.initPlot();
	gammaPlot.fillPlot();
	TH2* gammaClustermap = gammaPlot.getClustermap();

	TH2* resultClustermap = static_cast<TH2*>(lrClustermap->Clone());
	resultClustermap->Add(lClustermap, -1);
	resultClustermap->Add(rClustermap, -1);
	resultClustermap->Add(gammaClustermap, 1);
	resultClustermap->SetStats(0);
	resultClustermap->SetMinimum(-60);
	resultClustermap->SetMaximum(60);
	resultClustermap->SetTitle("D_{UL} Clustermap; Column; Row");


	TCanvas* canvas = new TCanvas("can", "", 2000, 1000);
	resultClustermap->Draw("COLZ0 min0");
	// canvas->SetTheta(0);
	// canvas->SetPhi(0);
	canvas->SaveAs("Plot/D_LR.png");
	return 0;
}