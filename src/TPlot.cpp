#include "TPlot.hpp"

#include "TROOT.h"
#include "TCanvas.h"
#include "TFrame.h"
#include "TLegend.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TColor.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TGraphMultiErrors.h"
#include "TGraphErrors.h"

#include "TConfig.hpp"

TCanvas* KEI::TPlot::initCanvas(const KEI::TConfig& config) {
	std::array<int, 2> size = {1000, 1000};
	TString canvasTitle = "CANVAS_TITLE";
	static int iCanvas = 0;
	if ( config.hasKey("CANVAS_SIZE") ) {
		size = config.getValue<int, 2>("CANVAS_SIZE");
	}
	if ( config.hasKey("CANVAS_TITLE") ) {
		canvasTitle = config.getValue<std::string>("CANVAS_TITLE");
	}
	TCanvas* canvas = new TCanvas(Form("canvas_%d", iCanvas), canvasTitle, size[0], size[1]);
	iCanvas++;

	return canvas;
}

TLegend* KEI::TPlot::initLegend(const KEI::TConfig& config) {
	std::array<double, 4> pos = {.7, .7, .9, .9};
	if ( config.hasKey("LEGEND_POSITION") ) {
		pos = config.getValue<double, 4>("LEGEND_POSITION");
	}
	TLegend* legend = new TLegend(pos[0], pos[1], pos[2], pos[3]);
	if ( config.hasKey("LEGEND_TITLE") ) {
		TString legendTitle = config.getValue<std::string>("LEGEND_TITLE");
		legend->SetHeader(legendTitle, "C");
	}
	if ( config.hasKey("LEGEND_FILL_COLOUR") ) {
		std::array<int, 3> rgb = config.getValue<int, 3>("LEGEND_FILL_COLOUR");
		int index = TColor::GetFreeColorIndex();
		new TColor(index, rgb[0] / 255., rgb[1] / 255., rgb[2] / 255.);
		if ( config.hasKey("LEGEND_FILL_ALPHA") ) {
			double alpha = config.getValue<double>("LEGEND_FILL_ALPHA");
			legend->SetFillColorAlpha(index, alpha);
		} else {
			legend->SetFillColor(index);
		}
	} else if ( config.hasKey("LEGEND_FILL_ALPHA") ) {
		double alpha = config.getValue<double>("LEGEND_FILL_ALPHA");
		legend->SetFillColorAlpha(kWhite, alpha);
	}

	return legend;
}

// Initialize a 1D histogram with the given configuration
// The histogram bins are determined by the "BIN" key in the configuration (default: {100, 0, 100})
TH1* KEI::TPlot::init1DHist(const KEI::TConfig& config) {
	std::array<double, 3> bins = {100, 0, 100};
	static int iHist = 0;

	std::string histName = config.hasKey("NAME") ? config.getValue<std::string>("NAME") : Form("hist1D_%d", iHist);

	if ( config.hasKey("BIN") ) {
		bins = config.getValue<double, 3>("BIN");
	}
	TH1* hist = new TH1D(static_cast<TString>(histName), "", bins[0], bins[1], bins[2]);

	iHist++;

	return hist;
}


// Initialize a 2D histogram with the given configuration
// The histogram bins are determined by the "BIN" key in the configuration (default: {100, 0, 100, 100, 0, 100})
TH2* KEI::TPlot::init2DHist(const KEI::TConfig& config) {
	static int i2DHist = 0;
	std::string histName = config.hasKey("NAME") ? config.getValue<std::string>("NAME") : Form("hist2D_%d", i2DHist);
	std::array<double, 6> bins = {100, 0, 100, 100, 0, 100};
	TH2* hist;
	if ( config.hasKey("BIN") ) {
		bins = config.getValue<double, 6>("BIN");
	}
	if ( config.hasKey("VAR_BIN_X") ) {
		std::vector<double> varBinsX = config.getValueVector<double>("VAR_BIN_X");
		hist = new TH2D(static_cast<TString>(histName), "", varBinsX.size() - 1, varBinsX.data(), bins[3], bins[4], bins[5]);
	} else if ( config.hasKey("VAR_BIN_Y") ) {
		std::vector<double> varBinsY = config.getValueVector<double>("VAR_BIN_Y");
		hist = new TH2D(static_cast<TString>(histName), "", bins[0], bins[1], bins[2], varBinsY.size() - 1, varBinsY.data());
	} else if ( config.hasKey("VAR_BIN_X") && config.hasKey("VAR_BIN_Y") ) {
		std::vector<double> varBinsX = config.getValueVector<double>("VAR_BIN_X");
		std::vector<double> varBinsY = config.getValueVector<double>("VAR_BIN_Y");
		hist = new TH2D(static_cast<TString>(histName), "", varBinsX.size() - 1, varBinsX.data(), varBinsY.size() - 1, varBinsY.data());
	} else {
		hist = new TH2D(static_cast<TString>(histName), "", bins[0], bins[1], bins[2], bins[3], bins[4], bins[5]);
	}
	i2DHist++;

	return hist;
}

// Initialize a function with the given configuration
// The function parameters are determined by the "FUNCTION" key in the configuration (default: {"x", "1"})
TF1* KEI::TPlot::initFunction(const KEI::TConfig& config) {
	static int iFunc = 0;
	std::array<double, 2> range = {0, 1};
	if ( config.hasKey("FIT_RANGE") ) {
		range = config.getValue<double, 2>("FIT_RANGE");
	}
	TString formula = config.hasKey("FIT_FUNC") ? config.getValue<std::string>("FIT_FUNC") : "x";
	TF1* func = new TF1(Form("func_%d", iFunc), formula, range[0], range[1]);
	if ( config.hasKey("FIT_PAR") ) {
		std::vector<double> par = getDoubleSetFromString(config.getValue<std::string>("FIT_PAR"));
		for ( size_t i = 0; i < par.size(); i++ ) {
			func->SetParameter(i, par[i]);
		}
	}
	if ( config.hasKey("LINE_WIDTH") ) {
		func->SetLineWidth(config.getValue<float>("LINE_WIDTH"));
	}
	if ( config.hasKey("LINE_COLOUR") ) {
		int index = TColor::GetFreeColorIndex();
		std::array<int, 3> rgb = config.getValue<int, 3>("LINE_COLOUR");
		new TColor(index, rgb[0] / 255., rgb[1] / 255., rgb[2] / 255.);
		func->SetLineColor(index);
	} else {
		func->SetLineColor(kBlue);
	}
	if ( config.hasKey("LINE_STYLE") ) {
		func->SetLineStyle(config.getValue<float>("LINE_STYLE"));
	}
	iFunc++;

	return func;
}

// Draw the plot on the canvas
void KEI::TPlot::drawPlot(TCanvas* canvas, TH1* plot, const KEI::TConfig& config) {
	if ( canvas == nullptr || plot == nullptr ) {
		return;
	}
	setLineWidth(plot, config);
	setLineColour(plot, config);
	setLineStyle(plot, config);

	if ( config.hasKey("Y_RANGE") ) {
		std::array<double, 2> yRange = config.getValue<double, 2>("Y_RANGE");
		plot->GetYaxis()->SetRangeUser(yRange[0], yRange[1]);
	}

	canvas->cd();
	TString option = "HIST";
	if ( config.hasKey("DRAW_OPTION") ) {
		option = config.getValue<std::string>("DRAW_OPTION");
		if ( option == "NO_DRAWING" ) {
		} else {
			plot->SetOption(option);
		}
	}
	plot->Draw(option);
}

// Draw 2D histogram on the canvas
void KEI::TPlot::drawPlot(TCanvas* canvas, TH2* plot, const KEI::TConfig& config) {
	if ( canvas == nullptr || plot == nullptr ) {
		return;
	}

	setLineColour(plot, config);
	setLineWidth(plot, config);
	setLineStyle(plot, config);

	canvas->cd();
	plot->Draw("COLZ");
	return;
}


void KEI::TPlot::drawPlot(TCanvas* canvas, TGraph* graph, const KEI::TConfig& config) {
	if ( canvas == nullptr || graph == nullptr ) {
		return;
	}

	if ( config.hasKey("LINE_COLOUR") ) {
		std::array<int, 3> rgb = config.getValue<int, 3>("LINE_COLOUR");
		int index = TColor::GetFreeColorIndex();
		new TColor(index, rgb[0] / 255., rgb[1] / 255., rgb[2] / 255.);
		graph->SetLineColor(index);
	}

	if ( config.hasKey("LINE_WIDTH") ) {
		graph->SetLineWidth(config.getValue<int>("LINE_WIDTH"));
	} else {
		graph->SetLineWidth(2);
	}

	if ( config.hasKey("LINE_STYLE") ) {
		graph->SetLineStyle(config.getValue<int>("LINE_STYLE"));
	}

	if ( config.hasKey("MARKER_STYLE") ) {
		graph->SetMarkerStyle(config.getValue<int>("MARKER_STYLE"));
	}
	if ( config.hasKey("MARKER_COLOUR") ) {
		std::array<int, 3> rgb = config.getValue<int, 3>("MARKER_COLOUR");
		int index = TColor::GetFreeColorIndex();
		new TColor(index, rgb[0] / 255., rgb[1] / 255., rgb[2] / 255.);
		graph->SetMarkerColor(index);
	}
	if ( config.hasKey("MARKER_SIZE") ) {
		graph->SetMarkerSize(config.getValue<float>("MARKER_SIZE"));
	}


	canvas->cd();

	if ( config.hasKey("DRAW_OPTION") ) {
		TString option = config.getValue<std::string>("DRAW_OPTION");
		if ( option == "NO_DRAWING" ) {
			return;
		} else {
			graph->SetOption(option);
		}
	} else {
		graph->SetOption("AP SAME");
	}
}

void KEI::TPlot::drawPlot(TCanvas* canvas, TGraphErrors* plot, const KEI::TConfig& config) {
	if ( canvas == nullptr || plot == nullptr ) {
		return;
	}
	if ( config.hasKey("LINE_COLOUR") ) {
		std::array<int, 3> rgb = config.getValue<int, 3>("LINE_COLOUR");
		int index = TColor::GetFreeColorIndex();
		new TColor(index, rgb[0] / 255., rgb[1] / 255., rgb[2] / 255.);
		plot->SetLineColor(index);
	}

	if ( config.hasKey("LINE_WIDTH") ) {
		plot->SetLineWidth(config.getValue<int>("LINE_WIDTH"));
	} else {
		plot->SetLineWidth(2);
	}

	if ( config.hasKey("LINE_STYLE") ) {
		plot->SetLineStyle(config.getValue<int>("LINE_STYLE"));
	}

	if ( config.hasKey("MARKER_COLOUR") ) {
		std::array<int, 3> rgb = config.getValue<int, 3>("MARKER_COLOUR");
		int index = TColor::GetFreeColorIndex();
		new TColor(index, rgb[0] / 255., rgb[1] / 255., rgb[2] / 255.);
		plot->SetMarkerColor(index);
	}
	if ( config.hasKey("MARKER_SIZE") ) {
		plot->SetMarkerSize(config.getValue<float>("MARKER_SIZE"));
	}
	if ( config.hasKey("MARKER_STYLE") ) {
		plot->SetMarkerStyle(config.getValue<int>("MARKER_STYLE"));
	}

	canvas->cd();

	if ( config.hasKey("DRAW_OPTION") ) {
		TString option = config.getValue<std::string>("DRAW_OPTION");
		if ( option == "NO_DRAWING" ) {
			return;
		} else {
			plot->SetOption(option);
		}
	} else {
		plot->SetOption("AP SAME");
	}
	plot->Draw();
}

void KEI::TPlot::drawPlot(TCanvas* canvas, TGraphMultiErrors* plot, const KEI::TConfig& config) {
	if ( canvas == nullptr || plot == nullptr ) {
		return;
	}
	int nErrors = plot->GetNYErrors();
	if ( config.hasKey("LINE_COLOUR") ) {
		std::array<int, 3> rgb = config.getValue<int, 3>("LINE_COLOUR");
		int index = TColor::GetFreeColorIndex();
		new TColor(index, rgb[0] / 255., rgb[1] / 255., rgb[2] / 255.);
		plot->SetLineColor(index);
		for ( int i = 0; i < nErrors; i++ ) {
			plot->SetLineColor(i, index);
		}
	}

	if ( config.hasKey("LINE_WIDTH") ) {
		double lineWidth = config.getValue<double>("LINE_WIDTH");
		plot->SetLineWidth(lineWidth);
		for ( int i = 0; i < nErrors; i++ ) {
			plot->SetLineWidth(i, lineWidth);
		}
	} else {
		plot->SetLineWidth(2);
		for ( int i = 0; i < nErrors; i++ ) {
			plot->SetLineWidth(i, 2);
		}
	}

	if ( config.hasKey("LINE_STYLE") ) {
		plot->SetLineStyle(config.getValue<int>("LINE_STYLE"));
	}

	if ( config.hasKey("MARKER_COLOUR") ) {
		std::array<int, 3> rgb = config.getValue<int, 3>("MARKER_COLOUR");
		int index = TColor::GetFreeColorIndex();
		new TColor(index, rgb[0] / 255., rgb[1] / 255., rgb[2] / 255.);
		plot->SetMarkerColor(index);
	}
	if ( config.hasKey("MARKER_SIZE") ) {
		plot->SetMarkerSize(config.getValue<float>("MARKER_SIZE"));
	}
	if ( config.hasKey("MARKER_STYLE") ) {
		plot->SetMarkerStyle(config.getValue<int>("MARKER_STYLE"));
	}
	plot->SetFillColorAlpha(1, kWhite, 0);

	canvas->cd();

	if ( config.hasKey("DRAW_OPTION") ) {
		TString option = config.getValue<std::string>("DRAW_OPTION");
		if ( option == "NO_DRAWING" ) {
			return;
		} else {
			plot->SetOption(option);
		}
	} else {
		plot->SetOption("AP SAME");
	}
	plot->Draw();
}

void KEI::TPlot::setAttribute(TH1* plot, const KEI::TConfig& config) {
	if ( plot == nullptr ) {
		return;
	}
	setTitle(plot, config);
	setRebin(plot, config);
	if ( config.hasKey("LABEL_SIZE") ) {
		std::array<double, 2> labelSize = config.getValue<double, 2>("LABEL_SIZE");
		plot->GetXaxis()->SetLabelSize(labelSize[0]);
		plot->GetYaxis()->SetLabelSize(labelSize[1]);
	}
	if ( config.hasKey("TITLE_SIZE") ) {
		std::array<double, 3> titleSize = config.getValue<double, 3>("TITLE_SIZE");
		plot->GetXaxis()->SetTitleSize(titleSize[1]);
		plot->GetYaxis()->SetTitleSize(titleSize[2]);
	}
	if ( config.hasKey("TITLE_OFFSET") ) {
		std::array<double, 2> titleOffset = config.getValue<double, 2>("TITLE_OFFSET");
		plot->GetTitle();
		plot->GetXaxis()->SetTitleOffset(titleOffset[0]);
		plot->GetYaxis()->SetTitleOffset(titleOffset[1]);
	}
	if ( config.hasKey("Y_RANGE") ) {
		std::array<double, 2> yRange = config.getValue<double, 2>("Y_RANGE");
		plot->SetMinimum(yRange[0]);
		plot->SetMaximum(yRange[1]);
	}
	return;
}

void KEI::TPlot::setAttribute(TH2* plot, const KEI::TConfig& config) {
	if ( plot == nullptr ) {
		return;
	}
	setTitle(plot, config);
	setRebin(plot, config);
	if ( config.hasKey("Z_RANGE") ) {
		std::array<double, 2> range = config.getValue<double, 2>("Z_RANGE");
		plot->SetMinimum(range[0]);
		plot->SetMaximum(range[1]);
	}
	return;
}

void KEI::TPlot::setAttribute(TGraph* graph, const KEI::TConfig& config) {
	if ( graph == nullptr ) {
		return;
	}
	TString title = "TITLE";
	if ( config.hasKey("TITLE") ) {
		title = config.getValue<std::string>("TITLE");
	}
	graph->SetTitle(title);
	if ( config.hasKey("LABEL_SIZE") ) {
		std::array<double, 2> labelSize = config.getValue<double, 2>("LABEL_SIZE");
		graph->GetXaxis()->SetLabelSize(labelSize[0]);
		graph->GetYaxis()->SetLabelSize(labelSize[1]);
	}
	if ( config.hasKey("TITLE_SIZE") ) {
		std::array<double, 3> titleSize = config.getValue<double, 3>("TITLE_SIZE");
		graph->GetXaxis()->SetTitleSize(titleSize[1]);
		graph->GetYaxis()->SetTitleSize(titleSize[2]);
	}
	if ( config.hasKey("TITLE_OFFSET") ) {
		std::array<double, 2> titleOffset = config.getValue<double, 2>("TITLE_OFFSET");
		graph->GetTitle();
		graph->GetXaxis()->SetTitleOffset(titleOffset[0]);
		graph->GetYaxis()->SetTitleOffset(titleOffset[1]);
	}
	if ( config.hasKey("Y_RANGE") ) {
		std::array<double, 2> yRange = config.getValue<double, 2>("Y_RANGE");
		graph->SetMinimum(yRange[0]);
		graph->SetMaximum(yRange[1]);
	}
	return;
}

void KEI::TPlot::setRebin(TH1* plot, const KEI::TConfig& config) {
	if ( plot == nullptr ) {
		return;
	}
	if ( config.hasKey("REBIN") ) {
		Int_t bin = config.getValue<Int_t>("REBIN");
		plot->Rebin(bin);
	}

}

void KEI::TPlot::setRebin(TH2* plot, const KEI::TConfig& config) {
	if ( plot == nullptr ) {
		return;
	}
	if ( config.hasKey("REBIN") ) {
		std::array<Int_t, 2> bin = config.getValue<Int_t, 2>("REBIN");
		plot->Rebin2D(bin[0], bin[1]);
	}

}

// Save the legend to the canvas
void KEI::TPlot::saveLegend(TCanvas* canvas, TLegend* legend) {
	if ( canvas == nullptr || legend == nullptr ) {
		return;
	}
	canvas->cd();
	legend->Draw("same");
	return;
}

// Save the canvas to a file
void KEI::TPlot::saveCanvas(TCanvas* canvas, const KEI::TConfig& config) {
	if ( canvas == nullptr ) {
		return;
	}

	TList* primitives = canvas->GetListOfPrimitives();
	if ( !primitives || primitives->GetSize() == 0 ) {
		// 객체가 없으면 그냥 저장하고 종료하거나 리턴
		if ( config.hasKey("OUTPUT_FILE") && config.hasKey("NAME") ) {
			// 경로 생성 및 저장 로직...
		}
		return;
	}

	TObject* obj = canvas->GetListOfPrimitives()->At(0);
	if ( obj->InheritsFrom("TFrame") ) {
		obj = canvas->GetListOfPrimitives()->At(1);
	}
	if ( obj->InheritsFrom("TH2") ) {
		setAttribute(dynamic_cast<TH2*>(obj), config);
	} else if ( obj->InheritsFrom("TH1") ) {
		setAttribute(dynamic_cast<TH1*>(obj), config);
	} else if ( obj->InheritsFrom("TGraph") ) {
		setAttribute(dynamic_cast<TGraph*>(obj), config);
	}

	setMargin(canvas, config);

	if ( config.hasKey("LOG_Z") && config.getValue<bool>("LOG_Z") ) {
		canvas->SetLogz();
	} else {
		canvas->SetLogz(0);
	}
	if ( config.hasKey("LOG_Y") && config.getValue<bool>("LOG_Y") ) {
		canvas->SetLogy();
	} else {
		canvas->SetLogy(0);
	}
	if ( config.hasKey("LOG_X") && config.getValue<bool>("LOG_X") ) {
		canvas->SetLogx();
	} else {
		canvas->SetLogx(0);
	}
	std::filesystem::path filePath = "canvas.png";
	if ( config.hasKey("PATH") ) {
		filePath = config.getValue<std::string>("PATH");
	}
	if ( !std::filesystem::exists(filePath.parent_path()) ) {
		std::filesystem::create_directories(filePath.parent_path());
	}
	canvas->SaveAs(filePath.c_str());
	return;
}

void KEI::TPlot::saveCanvas(TCanvas* canvas, const std::filesystem::path& filename, const KEI::TConfig& config) {
	if ( canvas == nullptr ) {
		return;
	}
	TObject* obj = canvas->GetListOfPrimitives()->At(0);
	if ( obj->InheritsFrom("TFrame") ) {
		if ( canvas->GetListOfPrimitives()->GetEntries() > 1 ) {
			obj = canvas->GetListOfPrimitives()->At(1);
		}
	}
	if ( obj->InheritsFrom("TH2") ) {
		setAttribute(dynamic_cast<TH2*>(obj), config);
	} else if ( obj->InheritsFrom("TH1") ) {
		setAttribute(dynamic_cast<TH1*>(obj), config);
	} else if ( obj->InheritsFrom("TGraph") ) {
		setAttribute(dynamic_cast<TGraph*>(obj), config);
	}
	setMargin(canvas, config);
	if ( config.hasKey("LOG_Z") && config.getValue<bool>("LOG_Z") ) {
		canvas->SetLogz();
	} else {
		canvas->SetLogz(0);
	}
	if ( config.hasKey("LOG_Y") && config.getValue<bool>("LOG_Y") ) {
		canvas->SetLogy();
	} else {
		canvas->SetLogy(0);
	}
	if ( config.hasKey("LOG_X") && config.getValue<bool>("LOG_X") ) {
		canvas->SetLogx();
	} else {
		canvas->SetLogx(0);
	}
	std::filesystem::path filePath = filename;
	std::string fileName = "canvas.png";
	if ( config.hasKey("PATH") ) {
		fileName = config.getValue<std::string>("PATH");
	}
	filePath /= fileName;
	if ( !std::filesystem::exists(filePath.parent_path()) ) {
		std::filesystem::create_directories(filePath.parent_path());
	}
	canvas->SaveAs(filePath.c_str());
	return;
}

void KEI::TPlot::setTitle(TH1* plot, const KEI::TConfig& config) {
	if ( plot == nullptr ) {
		return;
	}
	TString title = "TITLE";
	if ( config.hasKey("TITLE") ) {
		title = config.getValue<std::string>("TITLE");
	}
	plot->SetTitle(title);
	if ( config.hasKey("X_TITLE") ) {
		TString xTitle = config.getValue<std::string>("X_TITLE");
		plot->GetXaxis()->SetTitle(xTitle);
	}
	if ( config.hasKey("Y_TITLE") ) {
		TString yTitle = config.getValue<std::string>("Y_TITLE");
		plot->GetYaxis()->SetTitle(yTitle);
	}
	return;
}

void KEI::TPlot::setTitle(TH2* plot, const KEI::TConfig& config) {
	if ( plot == nullptr ) {
		return;
	}
	TString title = "TITLE";
	if ( config.hasKey("TITLE") ) {
		title = config.getValue<std::string>("TITLE");
	}
	plot->SetTitle(title);
	if ( config.hasKey("X_TITLE") ) {
		TString xTitle = config.getValue<std::string>("X_TITLE");
		plot->GetXaxis()->SetTitle(xTitle);
	}
	if ( config.hasKey("Y_TITLE") ) {
		TString yTitle = config.getValue<std::string>("Y_TITLE");
		plot->GetYaxis()->SetTitle(yTitle);
	}
	if ( config.hasKey("Z_TITLE") ) {
		TString zTitle = config.getValue<std::string>("Z_TITLE");
		plot->GetZaxis()->SetTitle(zTitle);
	}
	return;
}
void KEI::TPlot::setTitle(TGraph* plot, const KEI::TConfig& config) {
	if ( plot == nullptr ) {
		return;
	}
	TString title = "TITLE";
	if ( config.hasKey("TITLE") ) {
		title = config.getValue<std::string>("TITLE");
	}
	plot->SetTitle(title);
	if ( config.hasKey("X_TITLE") ) {
		TString xTitle = config.getValue<std::string>("X_TITLE");
		plot->GetXaxis()->SetTitle(xTitle);
	}
	if ( config.hasKey("Y_TITLE") ) {
		TString yTitle = config.getValue<std::string>("Y_TITLE");
		plot->GetYaxis()->SetTitle(yTitle);
	}
	return;
}

// Set the line color of the plot
void KEI::TPlot::setLineColour(TH1* plot, const KEI::TConfig& config) {
	if ( plot == nullptr ) {
		return;
	}
	int index = TColor::GetFreeColorIndex();
	if ( config.hasKey("LINE_COLOUR") ) {
		std::array<int, 3> rgb = config.getValue<int, 3>("LINE_COLOUR");
		new TColor(index, rgb[0] / 255., rgb[1] / 255., rgb[2] / 255.);
	} else {
		new TColor(index, 0x00 / 255., 0x00 / 255., 0xFF / 255.);
	}
	plot->SetLineColor(index);
	return;
}

// Set the line width of the plot
void KEI::TPlot::setLineWidth(TH1* plot, const KEI::TConfig& config) {
	if ( plot == nullptr ) {
		return;
	}
	int lineWidth = 2;
	if ( config.hasKey("LINE_WIDTH") ) {
		lineWidth = config.getValue<int>("LINE_WIDTH");
	}
	plot->SetLineWidth(lineWidth);
	return;
}

// Set the line style of the plot
void KEI::TPlot::setLineStyle(TH1* plot, const KEI::TConfig& config) {
	if ( plot == nullptr ) {
		return;
	}
	int lineStyle = 1;
	if ( config.hasKey("LINE_STYLE") ) {
		lineStyle = config.getValue<int>("LINE_STYLE");
	}
	plot->SetLineStyle(lineStyle);
	return;
}

void KEI::TPlot::setMargin(TCanvas* canvas, const KEI::TConfig& config) {
	if ( canvas == nullptr ) {
		return;
	}
	std::array<double, 4> margin = {0.1, 0.1, 0.1, 0.1};
	if ( config.hasKey("MARGIN") ) {
		margin = config.getValue<double, 4>("MARGIN");
	}
	if ( config.hasKey("LEFT_MARGIN") ) {
		margin[0] = config.getValue<double>("LEFT_MARGIN");

	}
	if ( config.hasKey("RIGHT_MARGIN") ) {
		margin[1] = config.getValue<double>("RIGHT_MARGIN");
	}
	if ( config.hasKey("TOP_MARGIN") ) {
		margin[2] = config.getValue<double>("TOP_MARGIN");
	}
	if ( config.hasKey("BOTTOM_MARGIN") ) {
		margin[3] = config.getValue<double>("BOTTOM_MARGIN");
	}
	canvas->SetMargin(margin[0], margin[1], margin[2], margin[3]);

	return;
}

std::string KEI::TPlot::getDateTime() {
	time_t now = time(NULL);
	struct tm* timeinfo = localtime(&now);

	std::ostringstream ss;
	ss << std::setw(2) << std::setfill('0') << timeinfo->tm_year - 100;
	ss << std::setw(2) << std::setfill('0') << timeinfo->tm_mon + 1;
	ss << std::setw(2) << std::setfill('0') << timeinfo->tm_mday;
	ss << "_";
	ss << std::setw(2) << std::setfill('0') << timeinfo->tm_hour;
	ss << std::setw(2) << std::setfill('0') << timeinfo->tm_min;
	ss << std::setw(2) << std::setfill('0') << timeinfo->tm_sec;
	std::string dateTime = ss.str();

	return dateTime;
}

std::vector<double> KEI::TPlot::getDoubleSetFromString(const std::string& str) {
	std::vector<double> result;
	std::istringstream iss(str);
	std::string token;

	while ( std::getline(iss, token, ' ') ) {
		try {
			result.push_back(std::stod(token));
		} catch ( const std::invalid_argument& e ) {
			std::cerr << "Invalid argument: " << e.what() << std::endl;
		} catch ( const std::out_of_range& e ) {
			std::cerr << "Out of range: " << e.what() << std::endl;
		}
	}

	return result;
}

std::vector<std::string> KEI::TPlot::getStringSetFromString(const std::string& str) {
	std::vector<std::string> result;
	std::istringstream iss(str);
	std::string token;

	while ( std::getline(iss, token, ' ') ) {
		result.push_back(token);
	}

	return result;
}