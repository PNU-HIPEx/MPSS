#include<filesystem>
#include "TFile.h"
#include "TH2.h"
#include "TLegend.h"
#include "TCanvas.h"

int main() {
	std::filesystem::path ULRPath = "/mnt/homes/ychoi/MPS/PLA/SIMULATION/ANALYSIS/ULR.root";
	TFile* ULRFile = new TFile(static_cast<TString>(ULRPath));
	TH2* ULRMap = static_cast<TH2D*>(ULRFile->Get("DEPOSIT_POSITION_XY"));

	std::filesystem::path ULRsPath = "/mnt/homes/ychoi/MPS/PLA/SIMULATION/ANALYSIS/ULRs.root";
	TFile* ULRsFile = new TFile(static_cast<TString>(ULRsPath));
	TH2* ULRsMap = static_cast<TH2D*>(ULRsFile->Get("DEPOSIT_POSITION_XY"));

	std::filesystem::path ULsRPath = "/mnt/homes/ychoi/MPS/PLA/SIMULATION/ANALYSIS/ULsR.root";
	TFile* ULsRFile = new TFile(static_cast<TString>(ULsRPath));
	TH2* ULsRMap = static_cast<TH2D*>(ULsRFile->Get("DEPOSIT_POSITION_XY"));

	std::filesystem::path UsLRPath = "/mnt/homes/ychoi/MPS/PLA/SIMULATION/ANALYSIS/UsLR.root";
	TFile* UsLRFile = new TFile(static_cast<TString>(UsLRPath));
	TH2* UsLRMap = static_cast<TH2D*>(UsLRFile->Get("DEPOSIT_POSITION_XY"));

	std::filesystem::path ULsRsPath = "/mnt/homes/ychoi/MPS/PLA/SIMULATION/ANALYSIS/ULsRs.root";
	TFile* ULsRsFile = new TFile(static_cast<TString>(ULsRsPath));
	TH2* ULsRsMap = static_cast<TH2D*>(ULsRsFile->Get("DEPOSIT_POSITION_XY"));

	std::filesystem::path UsLsRPath = "/mnt/homes/ychoi/MPS/PLA/SIMULATION/ANALYSIS/UsLsR.root";
	TFile* UsLsRFile = new TFile(static_cast<TString>(UsLsRPath));
	TH2* UsLsRMap = static_cast<TH2D*>(UsLsRFile->Get("DEPOSIT_POSITION_XY"));

	std::filesystem::path UsLRsPath = "/mnt/homes/ychoi/MPS/PLA/SIMULATION/ANALYSIS/UsLRs.root";
	TFile* UsLRsFile = new TFile(static_cast<TString>(UsLRsPath));
	TH2* UsLRsMap = static_cast<TH2D*>(UsLRsFile->Get("DEPOSIT_POSITION_XY"));

	std::filesystem::path UsLsRsPath = "/mnt/homes/ychoi/MPS/PLA/SIMULATION/ANALYSIS/UsLsRs.root";
	TFile* UsLsRsFile = new TFile(static_cast<TString>(UsLsRsPath));
	TH2* UsLsRsMap = static_cast<TH2D*>(UsLsRsFile->Get("DEPOSIT_POSITION_XY"));

	TH2* resultMap = static_cast<TH2*>(ULRMap->Clone());
	resultMap->Add(ULRsMap, -1);
	resultMap->Add(ULsRMap, -1);
	resultMap->Add(UsLRMap, -1);
	resultMap->Add(ULsRsMap, 1);
	resultMap->Add(UsLsRMap, 1);
	resultMap->Add(UsLRsMap, 1);
	resultMap->Add(UsLsRsMap, -1);
	resultMap->SetStats(0);
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
	canvas->SaveAs("Plot/Dreifachstreuung.png");

}