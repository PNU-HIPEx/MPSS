#include<filesystem>
#include<iostream>
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"

int main() {
	std::filesystem::path parentPath = "/mnt/homes/ychoi/MPS/SIMULATION/ANALYSIS/PLA/B3S4/";

	std::filesystem::path UPath = parentPath / "GAMMA_U_OXX.root";
	std::filesystem::path LPath = parentPath / "GAMMA_L_OXX.root";
	std::filesystem::path RPath = parentPath / "GAMMA_R_OXX.root";

	TFile* UFile = new TFile(std::string(UPath).c_str(), "READ");
	TFile* LFile = new TFile(std::string(LPath).c_str(), "READ");
	TFile* RFile = new TFile(std::string(RPath).c_str(), "READ");

	TH2D* UPosition = static_cast<TH2D*>(UFile->Get("DEPOSIT_POSITION_XY"));
	TH2D* LPosition = static_cast<TH2D*>(LFile->Get("DEPOSIT_POSITION_XY"));
	TH2D* RPosition = static_cast<TH2D*>(RFile->Get("DEPOSIT_POSITION_XY"));

	TH2D* ULRPosition = static_cast<TH2D*>(UPosition->Clone());
	ULRPosition->Add(LPosition);
	ULRPosition->Add(RPosition);

	TH1D* ULRSliceX = new TH1D("hist1", "Mean deposit position in -20 pix < y < 20 pix; Horizontal Axis [mm]; Entry", 1024, 0, 1024);
	TH1D* ULRSliceX_upper = new TH1D("hist1_upper", "", 1024, 0, 1024);
	TH1D* ULRSliceX_lower = new TH1D("hist1_upper", "", 1024, 0, 1024);
	TH1D* ULRSliceY = new TH1D("hist2", "Mean deposit position in -20 pix < x < 20 pix; Vertical Axis [mm]; Entry", 512, 0, 512);

	for ( size_t x = 0; x < 1024; x++ ) {
		for ( size_t y = 0; y < 512; y++ ) {
			Double_t entry = ULRPosition->GetBinContent(x, y);
			if ( y > 240 && y < 280 ) {
				ULRSliceX->Fill(x, entry);
			}
			if ( y > 10 && y < 50) {
				ULRSliceX_lower->Fill(x, entry);
			}
			if ( y > 462 && y < 502 ) {
				ULRSliceX_upper->Fill(x, entry);
			}
			if ( x > 492 && x < 532 ) {
				ULRSliceY->Fill(y, entry);
			}
		}
	}

	TCanvas* canvas = new TCanvas("canvas", "", 2000, 1000);
	ULRPosition->SetTitle("Mean deposit position; Horizontal Axis [mm]; Vertical Axis [mm]");
	ULRPosition->Rebin2D(8, 8);
	ULRPosition->Draw("COLZ0");
	canvas->SetMargin(.075, .125, .1, .1);
	canvas->SaveAs("Plot/ALPHA_LR.png");

	TCanvas* canvas2 = new TCanvas("canvas2", "", 1000, 1000);
	ULRSliceX->Rebin(8);
	ULRSliceX->SetMinimum(0);
	ULRSliceX->SetLineColor(kGreen);
	ULRSliceX->SetLineWidth(2);
	ULRSliceX->Draw("HIST");
	ULRSliceX_lower->Rebin(8);
	ULRSliceX_lower->SetLineColor(kRed);
	ULRSliceX_lower->SetLineWidth(2);
	ULRSliceX_lower->Draw("HIST SAME");
	ULRSliceX_upper->Rebin(8);
	ULRSliceX_upper->SetLineColor(kBlue);
	ULRSliceX_upper->SetLineWidth(2);
	ULRSliceX_upper->Draw("HIST SAME");
	canvas2->SaveAs("Plot/ALPHA_LR_SliceX.png");
	TCanvas* canvas3 = new TCanvas("canvas3", "", 1000, 1000);
	ULRSliceY->Rebin(4);
	ULRSliceY->SetMinimum(0);
	ULRSliceY->SetLineColor(kRed);
	ULRSliceY->SetLineWidth(2);
	ULRSliceY->Draw("HIST");
	canvas3->SaveAs("Plot/ALPHA_LR_SliceY.png");

	// UFile->Close();
	LFile->Close();
	RFile->Close();

	return 0;
}