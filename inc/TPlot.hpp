#ifndef __TPLOT_H__
#define __TPLOT_H__

#include<filesystem>
#include<vector>

class TCanvas;
class TLegend;
class TH1;
class TH2;
class TF1;
class TGraph;
class TGraphErrors;
class TGraphMultiErrors;

namespace KEI {
	class TConfig;

	class TPlot {
	public:
		static TCanvas* initCanvas(const KEI::TConfig& config);
		static TLegend* initLegend(const KEI::TConfig& config);
		static TH1* init1DHist(const KEI::TConfig& config);
		static TH2* init2DHist(const KEI::TConfig& config);
		static TF1* initFunction(const KEI::TConfig& config);
		static void drawPlot(TCanvas* canvas, TH1* plot, const KEI::TConfig& config);
		static void drawPlot(TCanvas* canvas, TH2* plot, const KEI::TConfig& config);
		static void drawPlot(TCanvas* canvas, TGraph* plot, const KEI::TConfig& config);
		static void drawPlot(TCanvas* canvas, TGraphErrors* plot, const KEI::TConfig& config);
		static void drawPlot(TCanvas* canvas, TGraphMultiErrors* plot, const KEI::TConfig& config);


		static void setAttribute(TH1* plot, const KEI::TConfig& config);
		static void setAttribute(TH2* plot, const KEI::TConfig& config);
		static void setAttribute(TGraph* plot, const KEI::TConfig& config);

		static void saveLegend(TCanvas* canvas, TLegend* legend);
		static void saveCanvas(TCanvas* canvas, const KEI::TConfig& config);
		static void saveCanvas(TCanvas* canvas, const std::filesystem::path& filename, const KEI::TConfig& config);

		static void setTitle(TH1* plot, const KEI::TConfig& config);
		static void setTitle(TH2* plot, const KEI::TConfig& config);
		static void setTitle(TGraph* plot, const KEI::TConfig& config);
		static void setLineColour(TH1* plot, const KEI::TConfig& config);
		static void setLineWidth(TH1* plot, const KEI::TConfig& config);
		static void setLineStyle(TH1* plot, const KEI::TConfig& config);
		static void setMargin(TCanvas* canvas, const KEI::TConfig& config);
		static void setCanvasStyle(TCanvas* canvas, const KEI::TConfig& config);
		static void setLogScale(TCanvas* canvas, const KEI::TConfig& config);
		static void setGrid(TCanvas* canvas, const KEI::TConfig& config);
		static std::string getDateTime();
		static void setRebin(TH1* plot, const KEI::TConfig& config);
		static void setRebin(TH2* plot, const KEI::TConfig& config);

		static std::vector<double> getDoubleSetFromString(const std::string& str);
		static std::vector<std::string> getStringSetFromString(const std::string& str);
	private:
	};

}

#endif