#include "Randomize.hh"
#include "G4RunManager.hh"

#include "TDetectorConstruction.hpp"
#include "TAnalysisManager.hpp"
#include "TPhysicsList.hpp"
#include "TActionInitialization.hpp"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "TConfig.hpp"
#include "config.hpp"

const std::filesystem::path sourcePath = SOURCE_DIR;
const std::string configFilePath = sourcePath / "Config/simulation.conf";

int main(int argc, char** argv) {
	KEI::TConfigFile config(configFilePath);

	// 몬테 카를로 시뮬레이션을 위한 랜덤 엔진 설정
	// CLHEP 라이브러리의 RanecuEngine을 사용하여 랜덤 엔진을 초기화합니다.
	CLHEP::RanecuEngine* RandomEngine = new CLHEP::RanecuEngine;
	G4Random::setTheEngine(RandomEngine);

	// G4RunManager를 생성하여 시뮬레이션 실행을 관리합니다.
	G4RunManager* runManager = new G4RunManager;

	// 사용자 정의 지오메트리 설정
	// TDetectorConstruction 클래스를 사용하여 환경 설정을 초기화합니다.
	runManager->SetUserInitialization(new TDetectorConstruction(config));
	// 분석 매니저 초기화
	// TAnalysisManager 클래스를 사용하여 분석 작업을 관리합니다.
	// 분석 매니저는 시뮬레이션 결과를 저장하고 처리하는 역할을 합니다.
	TAnalysisManager* analysisManager = new TAnalysisManager(config);

	// 물리학 리스트 설정
	// TPhysicsList 클래스를 사용하여 시뮬레이션에 필요한 물리학 프로세스를 정의합니다.
	// 이 리스트는 입자 상호작용 및 감쇠 과정을 포함합니다.
	TPhysicsList* physicsList = new TPhysicsList;
	runManager->SetVerboseLevel(0);
	runManager->SetUserInitialization(physicsList);

	// 초기 액션 설정
	// TActionInitialization 클래스를 사용하여 시뮬레이션의 초기 액션
	// (예: 이벤트 시작, 트래킹 등)을 정의합니다.
	runManager->SetUserInitialization(new TActionInitialization(config));

	// 시각화 관리자 초기화
	// G4VisManager를 사용하여 시뮬레이션의 시각화를 설정합니다.
	G4VisManager* visManager = new G4VisExecutive(argc, argv);
	visManager->Initialize();

	// UI 매니저 초기화
	// G4UImanager를 사용하여 사용자 인터페이스를 관리합니다.
	G4UImanager* uiManager = G4UImanager::GetUIpointer();

	// UI 실행기 초기화
	// G4UIExecutive를 사용하여 명령줄 인터페이스를 설정합니다.
	G4UIExecutive* ui = new G4UIExecutive(argc, argv);
	std::string visArg = (argc > 1) ? argv[1] : "";
	if ( visArg == "--vis" ) {
		// 시뮬레이션 초기화 명령 실행
		uiManager->ApplyCommand("/control/execute init_vis.mac");
		// UI 세션 시작
		ui->SessionStart();
	} else {
		// 시뮬레이션 초기화
		uiManager->ApplyCommand("/run/initialize");

		// 시뮬레이션 설정
		uiManager->ApplyCommand("/control/verbose 1");
		uiManager->ApplyCommand("/run/verbose 1");
		uiManager->ApplyCommand("/event/verbose 0");
		uiManager->ApplyCommand("/tracking/verbose 0");

		std::string activity = "1000";
		uiManager->ApplyCommand("/run/beamOn " + activity);

		// 분석 매니저 종료
		analysisManager->close();
	}

	delete ui;
	delete visManager;
	delete runManager;

	return 0;
}