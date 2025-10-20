#include <iostream>
#include <filesystem>
#include <string>

#include "Randomize.hh"
#include "G4RunManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"

#include "TDetectorConstruction.hpp"
#include "TAnalysisManager.hpp"
#include "TPhysicsList.hpp"
#include "TActionInitialization.hpp"

#include "TConfig.hpp"
#include "config.hpp"
#include "cppargs.hpp"

const std::filesystem::path sourcePath = SOURCE_DIR;
const std::string configFilePath = sourcePath / "config/simulation.conf";

ArgumentParser set_parse(int argc, char** argv) {
	ArgumentParser parser = ArgumentParser(argc, argv).setDescription("Drawing experiment plots");
	parser.add_argument("--vis").set_default("false").help("Visulaization option").add_finish();

	parser.parse_args();

	return parser;
}

int main(int argc, char** argv) {
	// 인자 파싱
	ArgumentParser parser = set_parse(argc, argv);
	// 컨피그 파일 열기
	KEI::TConfigFile config(configFilePath);

	// 몬테 카를로 시뮬레이션을 위한 랜덤 엔진 설정
	// CLHEP 라이브러리의 RanecuEngine을 사용하여 랜덤 엔진을 초기화합니다.
	long randomSeedNum = config.getConfig("FILE").getValue<long>("RANDOM_SEED");
	CLHEP::RanecuEngine* RandomEngine = new CLHEP::RanecuEngine;
	G4Random::setTheEngine(RandomEngine);
	G4Random::setTheSeed(randomSeedNum);

	// G4RunManager를 생성
	G4RunManager* runManager = new G4RunManager;

	// 사용자 정의 지오메트리 설정
	runManager->SetUserInitialization(new TDetectorConstruction(config));
	
	// 분석 매니저 초기화
	// TAnalysisManager 클래스를 사용하여 분석 작업을 관리합니다.
	// TAnalysisManager* analysisManager = new TAnalysisManager(config);

	// 물리학 리스트 설정
	TPhysicsList* physicsList = new TPhysicsList;
	runManager->SetVerboseLevel(0); // 물리학 리스트의 정보 출력 레벨 설정
	runManager->SetUserInitialization(physicsList);

	// 초기 액션 설정
	runManager->SetUserInitialization(new TActionInitialization(config));

	// UI 매니저 초기화
	G4UImanager* uiManager = G4UImanager::GetUIpointer();
	

	if ( parser.get_value<bool>("vis") ){
		// 아래와 같이 고치면 make가 완료됨.
		G4VisManager* visManager = new G4VisExecutive("quiet");
		visManager->Initialize();

		// UI 실행기 초기화
		// G4UIExecutive를 사용하여 명령줄 인터페이스를 설정합니다.
		G4UIExecutive* ui = new G4UIExecutive(argc, argv);

		// 시뮬레이션 초기화 명령 실행
		uiManager->ApplyCommand("/control/execute init_vis.mac");
		// UI 세션 시작
		ui->SessionStart();

		delete ui;
		delete visManager;
	} else {
		// 시뮬레이션 초기화
		uiManager->ApplyCommand("/run/initialize");

		// 시뮬레이션 설정
		uiManager->ApplyCommand("/control/verbose 1");
		uiManager->ApplyCommand("/run/verbose 1");
		uiManager->ApplyCommand("/event/verbose 0");
		uiManager->ApplyCommand("/tracking/verbose 0");

		int activity = config.getConfig("ENVIRONMENT").getValue<int>("ACTIVITY");
		uiManager->ApplyCommand("/run/beamOn " + std::to_string(activity));
	}

	delete runManager;

	return 0;
}