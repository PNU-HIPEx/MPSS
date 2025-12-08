#ifndef TANALYSISHELPER_HPP
#define TANALYSISHELPER_HPP

#include "G4AnalysisManager.hh"
#include "G4String.hh"

struct TAnalysisHelper {
	static inline void FillH1(const G4String& name, double value) {
		G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
		int id = analysisManager->GetH1Id(name);
		if (id >= 0) {
			analysisManager->FillH1(id, value);
		}
	}
	static inline void FillH2(const G4String& name, double x, double y) {
		G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
		int id = analysisManager->GetH2Id(name);
		if (id >= 0) {
			analysisManager->FillH2(id, x, y);
		}
	}
	static inline void FillH3(const G4String& name, double x, double y, double z) {
		G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
		int id = analysisManager->GetH3Id(name);
		if (id >= 0) {
			analysisManager->FillH3(id, x, y, z);
		}
	}
};

#endif // TANALYSISHELPER_HPP