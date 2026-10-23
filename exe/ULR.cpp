#include<filesystem>
#include "TH2.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"

int main() {
	// Alpha U
	std::filesystem::path alphaUPath = "/mnt/homes/ychoi/MPS/PLA/SIMULATION/ANALYSIS/U.root";
	TFile* alphaUFile = new TFile(static_cast<TString>(alphaUPath));
	TH2* alphaUMap = static_cast<TH2D*>(alphaUFile->Get("DEPOSIT_POSITION_XY"));

	// Gamma U
	std::filesystem::path gammaUPath = "/mnt/homes/ychoi/MPS/PLA/SIMULATION/ANALYSIS/GAMMA_U_XXX.root";
	TFile* gammaUFile = new TFile(static_cast<TString>(gammaUPath));
	TH2* gammaUMap = static_cast<TH2D*>(gammaUFile->Get("DEPOSIT_POSITION_XY"));

	// Gamma L
	std::filesystem::path gammaLPath = "/mnt/homes/ychoi/MPS/PLA/SIMULATION/ANALYSIS/GAMMA_L_XXX.root";
	TFile* gammaLFile = new TFile(static_cast<TString>(gammaLPath));
	TH2* gammaLMap = static_cast<TH2D*>(gammaLFile->Get("DEPOSIT_POSITION_XY"));

	// Gamma R
	std::filesystem::path gammaRPath = "/mnt/homes/ychoi/MPS/PLA/SIMULATION/ANALYSIS/GAMMA_R_XXX.root";
	TFile* gammaRFile = new TFile(static_cast<TString>(gammaRPath));
	TH2* gammaRMap = static_cast<TH2D*>(gammaRFile->Get("DEPOSIT_POSITION_XY"));

	TH2* resultMap = static_cast<TH2*>(alphaUMap->Clone());
	resultMap->Add(gammaUMap);
	resultMap->Add(gammaLMap);
	resultMap->Add(gammaRMap);
	resultMap->SetStats(0);
	resultMap->Rebin2D(8, 8);
	resultMap->SetTitle("Deposit position corresponding to U#lower[0.2]{#alpha#gamma}#lower[0.2]{#alpha#gamma}#lower[0.2]{#alpha#gamma}; Column [mm]; Row [mm]; Entry");

	TLegend* legend = new TLegend(.675, .6, .875, .9);
	legend->SetFillColorAlpha(kWhite, 0.3);
	legend->AddEntry(resultMap, Form("Entry: %0.f", resultMap->GetEffectiveEntries()), "");
	legend->AddEntry(resultMap, Form("Mean X: %.3f", resultMap->GetMean(1)), "");
	legend->AddEntry(resultMap, Form("Mean Y: %.3f", resultMap->GetMean(2)), "");
	legend->AddEntry(resultMap, Form("StdDev X: %.3f", resultMap->GetStdDev(1)), "");
	legend->AddEntry(resultMap, Form("StdDev Y: %.3f", resultMap->GetStdDev(2)), "");

	TCanvas* canvas = new TCanvas("can", "", 2000, 1000);
	resultMap->Draw("COLZ0 min0");
	canvas->SetTheta(0);
	canvas->SetPhi(0);
	canvas->SetMargin(.075, .125, .1, .1);

	legend->Draw("SAME");
	canvas->SaveAs("Plot/ULR.png");

	TFile* outputFile = new TFile("/mnt/homes/ychoi/MPS/PLA/SIMULATION/ANALYSIS/ULR.root", "RECREATE");
	resultMap->Write();
	outputFile->Close();

	return 0;
}