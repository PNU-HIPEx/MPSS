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
#include "cppargs.hpp"
#include <ctime>
#include <iostream>
#include <filesystem>
#include <string>
#include <cstdlib>
#include <vector>
#include <cctype>

const std::filesystem::path sourcePath = SOURCE_DIR;
const std::string configFilePath = sourcePath / "config/simulation.conf";

ArgumentParser set_parse(int argc, char** argv) {
	ArgumentParser parser = ArgumentParser(argc, argv).setDescription("Drawing experiment plots");
	parser.add_argument("--vis").set_default("false").help("Visulaization option").add_finish();
	parser.add_argument("--seed").set_default("").help("Random seed (integer)").add_finish();

	parser.parse_args();

	return parser;
}

// 소문자화
static std::string lower(std::string s){
  for(char& c: s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  return s;
}

// "true/false/1/0/on/off/yes/no" 파싱
static bool parse_bool_token(const std::string& v, bool& ok){
  std::string t = lower(v);
  if (t=="1"||t=="true"||t=="on"||t=="yes"){ ok=true; return true; }
  if (t=="0"||t=="false"||t=="off"||t=="no"){ ok=true; return false; }
  ok=false; return false;
}

// --seed, -s, --seed=NUM, --vis, --vis=BOOL, "--vis BOOL" 지원
// 파서에 넘길 argv에서 이 둘은 제거하여 파서 오류를 근본적으로 회피
static void extractSeedVisAndFilterArgs(int argc, char** argv,
                                        long& seed_out, bool& vis_out,
                                        int& new_argc, std::vector<char*>& new_argv){
  seed_out = 1;   // 기본 seed
  vis_out  = false;
  new_argv.clear();
  new_argv.push_back(argv[0]);
  new_argc = 1;

  for(int i=1;i<argc;++i){
    std::string a = argv[i];

    // --seed, -s (공백 인자)
    if (a=="--seed" || a=="-s"){
      if (i+1<argc){
        char* endp=nullptr;
        long v = std::strtol(argv[i+1], &endp, 10);
        if (endp && *endp=='\0' && v>0) seed_out = v;
        ++i; // 다음 토큰 소비
      }
      continue; // 파서에 넘기지 않음
    }
    // --seed=NUM
    if (a.rfind("--seed=",0)==0){
      const std::string val = a.substr(7);
      char* endp=nullptr;
      long v = std::strtol(val.c_str(), &endp, 10);
      if (endp && *endp=='\0' && v>0) seed_out = v;
      continue;
    }

    // --vis (플래그)
    if (a=="--vis"){
      // 바로 뒤에 값이 있고, 그것이 bool 토큰이면 그 값 사용
      if (i+1<argc){
        bool ok=false; bool b = parse_bool_token(argv[i+1], ok);
        if (ok){ vis_out = b; ++i; continue; }
      }
      // 값이 없으면 true로 간주
      vis_out = true;
      continue;
    }
    // --vis=VALUE
    if (a.rfind("--vis=",0)==0){
      bool ok=false; vis_out = parse_bool_token(a.substr(6), ok);
      if (!ok) vis_out = true; // 값이 이상하면 true로 처리
      continue;
    }

    // 나머지는 그대로 파서에 전달
    new_argv.push_back(argv[i]);
    ++new_argc;
  }
}

int main(int argc, char** argv) {
	// 1) seed/vis를 먼저 직접 파싱하고, 파서에 넘길 argv를 필터링
  	int argc2=0; std::vector<char*> argv2;
	long seed=1; bool vis=false;
	extractSeedVisAndFilterArgs(argc, argv, seed, vis, argc2, argv2);

	// 2) 필터링된 인자로만 기존 ArgumentParser 실행
	ArgumentParser parser = set_parse(argc2, argv2.data());
	// ArgumentParser parser = set_parse(argc, argv);

	KEI::TConfigFile config(configFilePath);

	// 몬테 카를로 시뮬레이션을 위한 랜덤 엔진 설정
	// CLHEP 라이브러리의 RanecuEngine을 사용하여 랜덤 엔진을 초기화합니다.
	CLHEP::RanecuEngine* RandomEngine = new CLHEP::RanecuEngine;
	G4Random::setTheEngine(RandomEngine);
	G4Random::setTheSeed(seed);



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

	// UI 매니저 초기화
	// G4UImanager를 사용하여 사용자 인터페이스를 관리합니다.
	G4UImanager* uiManager = G4UImanager::GetUIpointer();
	

	// if ( parser.get_value<bool>("vis") )
	if (vis) 
	{
		// 시각화 관리자 초기화
		// G4VisManager를 사용하여 시뮬레이션의 시각화를 설정합니다.
		// Geant4 11.1.2 에서는 밑의 생성자가 존재하지 않음 -> make 실패
		// G4VisManager* visManager = new G4VisExecutive(argc, argv);

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

		// 분석 매니저 종료
		analysisManager->close();
		// delete analysisManager;
		std::cout << "Using random seed = " << seed << std::endl;
		G4Random::showEngineStatus();  // Geant4 엔진 상태 출력
	}

	delete runManager;

	return 0;
}