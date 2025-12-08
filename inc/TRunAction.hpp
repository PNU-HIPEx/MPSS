#ifndef __TRUNACTION__
#define __TRUNACTION__

#include "G4UserRunAction.hh"
#include "TConfig.hpp"
#include "cpptqdm.h"
#include "G4AnalysisManager.hh"

class TRunAction : public G4UserRunAction {
public: // 생성자
	TRunAction() = default;
	TRunAction(const KEI::TConfigFile& config);
	~TRunAction() override = default;
private: // 데이터 멤버
	KEI::TConfigFile mConfig; // Config 파일
	ProgressBar* mProgressBar = nullptr; // tqdm용 포인터
public: // 멤버 함수
	void BeginOfRunAction(const G4Run*) override;
	void EndOfRunAction(const G4Run*) override;
};

#endif