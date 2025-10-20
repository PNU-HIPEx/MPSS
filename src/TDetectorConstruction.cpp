#include "TDetectorConstruction.hpp"

#include "G4SystemOfUnits.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "map"

#include "config.hpp"

const G4double AIR_DENSITY = 1.2929e-03 * g / cm3;
const G4double N_mass = 14.01 * g / mole;
const G4double O_mass = 16.00 * g / mole;

const G4double C_mass = 12.01 * g / mole;
const G4double H_mass = 1.008 * g / mole;


const std::filesystem::path sourceDir = SOURCE_DIR;
const std::string collimatorPath = sourceDir / "config/collimator.stl";


TDetectorConstruction::TDetectorConstruction(const KEI::TConfigFile& config) : G4VUserDetectorConstruction() {
	mAirPressure = config.getConfig("ENVIRONMENT").getValue<double>("AIR_PRESSURE");
}

G4VPhysicalVolume* TDetectorConstruction::Construct() {
	mNist = G4NistManager::Instance();
	getWorld();
	getCollimator();
	getShield();
	getDetector();

	return mWorld;
}

void TDetectorConstruction::getWorld() {
	G4double worldX = 50 * mm;
	G4double worldY = 50 * mm;
	G4double worldZ = 35 * mm;
	// G4double worldZ = 50 * mm;

	G4Box* solidWorld = new G4Box("World", .5 * worldX, .5 * worldY, .5 * worldZ); // 중심에서 부터의 거리라서 원하는 길이에서 0.5를 곱해주어야 함.

	G4Material* worldMaterial = new G4Material("worldMaterial", mAirPressure * AIR_DENSITY, 2);
	G4Element* elN = new G4Element("Nitrogen", "N", 7, N_mass);
	G4Element* elO = new G4Element("Oxygen", "O", 8, O_mass);
	worldMaterial->AddElement(elN, .7);
	worldMaterial->AddElement(elO, .3);

	mWorldLogical = new G4LogicalVolume(solidWorld, worldMaterial, "World");

	mWorld = new G4PVPlacement(0, G4ThreeVector(), mWorldLogical, "World", 0, false, 0, true);
}


void TDetectorConstruction::getCollimator() {
	/*
		GEANT4에서 사물을 배치하는 방법
		1. 사물의 형태인 솔리드 볼륨 먼저 제작한다. (G4Solid)
		솔리드 볼륨 : 형태, 기하학적 정보만 정의된 상태, G4Box, G4Tubs, G4Sphere등 G4 자체적으로 정의된 class를 사용
		
		2. 앞서 제작한 형태에 재질을 추가한 논리 볼륨을 만든다. (G4LogicalVolume)
		// 로직 볼륨 : 솔리드 볼륨에 물질과 속성(색상, 민감도??)를 부여함. 아직 물리적으로 배치되지 않음 = 월드에 없음
		계층구조상 어미 또는 자식을 가질 수 있다...

		3. 논리 볼륨에 위치와 방향을 지정하여 물리 볼륨을 배치한다. (G4PVPlacement)
		// 로직 볼륨에 좌표와 방향을 결정하여 실제 시뮬레이션 공간에 배치

	*/

	// 형태 제작 튜토리얼
	
	// 솔리드 볼륨 제작
	// G4Box_1 : 작은 방사선원 끼우는 곳 -> 큰박스
	G4double X_BoxSmallSource = 30 * mm;
	G4double Y_BoxSmallSource = 17 * mm;
	G4double Z_BoxSmallSource = 15 * mm;
	// 박스를 제작한다. 매개변수는 이름, x 크기의 절반, y 크기의 절반, z 크기의 절반이다.
	// 솔리드 볼륨
	G4Box* solid_BoxSmallSource = new G4Box("BoxSmallSource", .5 * X_BoxSmallSource, .5 * Y_BoxSmallSource, .5 * Z_BoxSmallSource);

	// G4Box_2 : 큰 방사선원 끼우기 곳을 위해 추가할 박스 -> 큰 방사선원을 끼우기 위한 깊이차이 1mm를 추가해주기 위한 박스
	G4double X_BoxBigSource = 30 * mm;
	G4double Y_BoxBigSource = 1 * mm;
	G4double Z_BoxBigSource = 15 * mm;
	// 솔리드 볼륨
	G4Box* solid_BoxBigSource = new G4Box("BoxBigSource", .5 * X_BoxBigSource, .5 * Y_BoxBigSource, .5 * Z_BoxBigSource);

	// G4Tubs
	// 방사선원 끼우는 곳, 입자가 지나갈 곳
	// 공통
	G4double InnerRadius = 0 * mm;
	G4double Height_tubsSource = 3 * mm;
	G4double StartAngle_tubsSources = 0 * deg;
	G4double EndAngle_tubsSources = 360 * deg;
	
	// 작은 방사선원
	G4double OuterRadius_tubsSmallSource = 6.5 * mm;
	G4double ApertureRadius_SmallSource_Small = 1.5 * mm;
	G4double ApertureRadius_SmallSource_Medium = 2 * mm;
	// G4double ApertureRadius_SmallSource_Big = 2.5 * mm;
	// Smaller라는 단어의 의미는 큰 방사선원의 통로가 더 커서 작은 방사선원을 끼우는 공간이 더 작음(2.5)을 의미
	// G4double Height_tubsSourceSmaller = 3 * mm;
	G4double Height_Aperture4SmallSource = 15 * mm;

	// 큰 방사선원
	G4double OuterRadius_tubsBigSource = 9.1 * mm;
	// G4double ApertureRadius_BigSource_Small = 1 * mm;
	G4double ApertureRadius_BigSource_Big = 1.5 * mm;
	G4double Height_Aperture4BigSource = 17.01 * mm;

	// 콜리메이터 좌우구멍으로 새어나가는 것 및 콜리메이터 설치된 부분의 밑으로 새어나오는 현상때문에 다음의 수정사항이 필요함
	// 콜리메이터의 바닥부분으로 2mm 추가, 디텍터는 늘어난 콜리메이터 바닥으로부터 10mm 떨어진 곳에 위치, 좌우에 작은 방사선원을 끼울 홈이 2.5mm에서 3mm로 고정 및 변경, 콜리메이팅 길이도 3mm로 변경 ->축으로 총 3(small 방사선원 끼울 홈)+3(콜리메이팅 길이)+3(big 방사선원 콜리메이팅 지름)+3(대칭)+3(대칭) = 15mm


	// Tubs를 제작한다. 매개변수는 이름, 내경, 외경, 높이의 절반, 시작 각도, 총 각도이다.
	// 방사선원 솔리드 볼륨
	// 작은 방사선원 끼우는 홈
	G4Tubs* solid_TubsSmallSource = new G4Tubs("Tubs", InnerRadius, OuterRadius_tubsSmallSource, .5 * Height_tubsSource, StartAngle_tubsSources, EndAngle_tubsSources);
	
	// 밑의 큰 방사선원이 큰 통로 일때 사용
	// G4Tubs* solid_TubsSmallSourceSmaller = new G4Tubs("Tubs", InnerRadius, OuterRadius_tubsSmallSource, .5 * Height_tubsSourceSmaller, StartAngle_tubsSources, EndAngle_tubsSources);
	
	// 큰 방사선원 끼우는 홈
	G4Tubs* solid_TubsBigSource = new G4Tubs("Tubs", InnerRadius, OuterRadius_tubsBigSource, .5 * Height_tubsSource, StartAngle_tubsSources, EndAngle_tubsSources);

	
	// 콜리메이팅 통로 솔리드 볼륨 
	// 방사선원 <> height_ 
	// 통로 <> ApertureRadius
	
	// 작은 방사선원 - 작은 통로
	G4Tubs* solid_SmallSourceSmallAperture = new G4Tubs("Tubs", InnerRadius, ApertureRadius_SmallSource_Small, .5 * Height_Aperture4SmallSource, StartAngle_tubsSources, EndAngle_tubsSources);
	// 작은 방사선원 - 중간 통로
	G4Tubs* solid_SmallSourceMediumAperture = new G4Tubs("Tubs", InnerRadius, ApertureRadius_SmallSource_Medium, .5 * Height_Aperture4SmallSource, StartAngle_tubsSources, EndAngle_tubsSources);
	// 작은 방사선원 - 큰 통로
	// G4Tubs* solid_SmallSourceBigAperture = new G4Tubs("Tubs", InnerRadius, ApertureRadius_SmallSource_Big, .5 * Height_Aperture4SmallSource, StartAngle_tubsSources, EndAngle_tubsSources);

	// 큰 방사선원 - 작은 통로
	// G4Tubs* solid_BigSourceSmallAperture = new G4Tubs("Tubs", InnerRadius, ApertureRadius_BigSource_Small, .5 * Height_Aperture4BigSource, StartAngle_tubsSources, EndAngle_tubsSources);
	// 큰 방사선원 - 큰 통로
	G4Tubs* solid_BigSourceBigAperture = new G4Tubs("Tubs", InnerRadius, ApertureRadius_BigSource_Big, .5 * Height_Aperture4BigSource, StartAngle_tubsSources, EndAngle_tubsSources);

	// 시뮬레이션에 사용할 콜리메이터 솔리드볼륨 제작
	// 1. 큰박스에 작은 방사선원을 끼울곳을 만든다 (3 mm 깊이)
	G4ThreeVector pos_SmallSource(0, 0, .5 * (Z_BoxSmallSource - Height_tubsSource));
	// G4ThreeVector pos_SmallSource_smaller(0, 0, .5 * (Z_BoxSmallSource - Height_tubsSourceSmaller));
	
	// 
	G4SubtractionSolid* BoxSubSmallSource_temp = new G4SubtractionSolid("", solid_BoxSmallSource, solid_TubsSmallSource, 0, pos_SmallSource);
	G4SubtractionSolid* BoxSubSmallSource = new G4SubtractionSolid("", BoxSubSmallSource_temp, solid_TubsSmallSource, 0, -pos_SmallSource);

	// 1'. 박스에 작은 방사선원을 끼울 곳을 만든다(2.5 mm 깊이로)
	// G4SubtractionSolid* BoxSubSmallSource_smaller_temp = new G4SubtractionSolid("", solid_BoxSmallSource, solid_TubsSmallSourceSmaller, 0, pos_SmallSource_smaller);
	// G4SubtractionSolid* BoxSubSmallSource_smaller = new G4SubtractionSolid("", BoxSubSmallSource_smaller_temp, solid_TubsSmallSourceSmaller, 0, -pos_SmallSource_smaller);

	// 2. 작은 방사선원에 대한 Aperture를 뚫는다.
	G4ThreeVector pos_Aperture4Source(0, 0, 0);

	G4SubtractionSolid* SubSmallAperture4SmallSource_temp = new G4SubtractionSolid("", BoxSubSmallSource, solid_SmallSourceSmallAperture, 0, pos_Aperture4Source);
	G4SubtractionSolid* SubMediumAperture4SmallSource_temp = new G4SubtractionSolid("", BoxSubSmallSource, solid_SmallSourceMediumAperture, 0, pos_Aperture4Source);
	// G4SubtractionSolid* SubBigAperture4SmallSource = new G4SubtractionSolid("", BoxSubSmallSource, solid_SmallSourceBigAperture, 0, pos_Aperture4Source);

	// G4SubtractionSolid* SubSmallAperture4SmallSource_smaller = new G4SubtractionSolid("", BoxSubSmallSource_smaller, solid_SmallSourceSmallAperture, 0, pos_Aperture4Source);
	// G4SubtractionSolid* SubMediumAperture4SmallSource_smaller = new G4SubtractionSolid("", BoxSubSmallSource_smaller, solid_SmallSourceMediumAperture, 0, pos_Aperture4Source);
	// G4SubtractionSolid* SubBigAperture4SmallSource_smaller = new G4SubtractionSolid("", BoxSubSmallSource_smaller, solid_SmallSourceBigAperture, 0, pos_Aperture4Source);

	// 2.5 큰박스에서 큰 방사선원을 끼우기 위한 홈을 2mm를 만든다.
	G4ThreeVector pos_BigBoxSubBigSource(0, .5 * (Y_BoxSmallSource - 2), 0);
	// 오일러 각 ZXZ
	G4RotationMatrix* rot_BigSource = new G4RotationMatrix(0 * deg, 90 * deg, 0 * deg);

	G4Tubs* BigBoxSubBigSource = new G4Tubs("", InnerRadius, OuterRadius_tubsBigSource, 0.5 * 2, StartAngle_tubsSources, EndAngle_tubsSources);
	
	G4SubtractionSolid* SubSmallAperture4SmallSource = new G4SubtractionSolid("", SubSmallAperture4SmallSource_temp, BigBoxSubBigSource, rot_BigSource, pos_BigBoxSubBigSource);
	G4SubtractionSolid* SubMediumAperture4SmallSource = new G4SubtractionSolid("", SubMediumAperture4SmallSource_temp, BigBoxSubBigSource, rot_BigSource, pos_BigBoxSubBigSource);

	// 3. 큰 방사선원을 끼우기 위한 박스를 만든다. -> 큰 방사선원을 끼우기 위한 깊이차이 1mm를 추가해주기 위한 박스
	G4ThreeVector pos_BigSource(0, 0, 0);
	G4SubtractionSolid* solid_4bigSource = new G4SubtractionSolid("", solid_BoxBigSource, solid_TubsBigSource, rot_BigSource, pos_BigSource);
	
	// 4. 기존 collimator와 합친다
	G4ThreeVector pos_box2(0, 0.5 * (Y_BoxSmallSource + Y_BoxBigSource), 0);

	G4UnionSolid* SubSmallAperture4SmallSource_temp1 = new G4UnionSolid("", SubSmallAperture4SmallSource, solid_4bigSource, 0, pos_box2);
	G4UnionSolid* SubMediumAperture4SmallSource_temp1 = new G4UnionSolid("", SubMediumAperture4SmallSource, solid_4bigSource, 0, pos_box2);
	// G4UnionSolid* SubBigAperture4SmallSource_temp1 = new G4UnionSolid("", SubBigAperture4SmallSource, solid_4bigSource, 0, pos_box2);

	// G4UnionSolid* SubSmallAperture4SmallSource_smaller_temp1 = new G4UnionSolid("", SubSmallAperture4SmallSource_smaller, solid_4bigSource, 0, pos_box2);
	// G4UnionSolid* SubMediumAperture4SmallSource_smaller_temp1 = new G4UnionSolid("", SubMediumAperture4SmallSource_smaller, solid_4bigSource, 0, pos_box2);
	// G4UnionSolid* SubBigAperture4SmallSource_smaller_temp1 = new G4UnionSolid("", SubBigAperture4SmallSource_smaller, solid_4bigSource, 0, pos_box2);

	// 5. 큰 방사선원에 대한 Aperture를 뚫는다
	G4SubtractionSolid* collimator_30_30 = new G4SubtractionSolid("", SubSmallAperture4SmallSource_temp1, solid_BigSourceBigAperture, rot_BigSource, pos_Aperture4Source);
	G4SubtractionSolid* collimator_30_40 = new G4SubtractionSolid("", SubMediumAperture4SmallSource_temp1, solid_BigSourceBigAperture, rot_BigSource, pos_Aperture4Source);
	// G4SubtractionSolid* collimator_30_50 = new G4SubtractionSolid("", SubBigAperture4SmallSource_temp1, solid_BigSourceSmallAperture, rot_BigSource, pos_Aperture4Source);

	// G4SubtractionSolid* collimator_30_30 = new G4SubtractionSolid("", SubSmallAperture4SmallSource_smaller_temp1, solid_BigSourceBigAperture, rot_BigSource, pos_Aperture4Source);
	// G4SubtractionSolid* collimator_30_40 = new G4SubtractionSolid("", SubMediumAperture4SmallSource_smaller_temp1, solid_BigSourceBigAperture, rot_BigSource, pos_Aperture4Source);
	// G4SubtractionSolid* collimator_30_50 = new G4SubtractionSolid("", SubBigAperture4SmallSource_smaller_temp1, solid_BigSourceBigAperture, rot_BigSource, pos_Aperture4Source);
	
	// GUI Test 코드
	// G4SubtractionSolid* testSoild = new G4SubtractionSolid("", , solid_BoxBigSource, 0, pos_BoxBigSource);
	// G4Material* Al = mNist->FindOrBuildMaterial("G4_Al");
	// G4LogicalVolume* test = new G4LogicalVolume(collimator_30_50, Al, "test");
	// G4RotationMatrix* rot_test = new G4RotationMatrix(0 * deg, 0 * deg, 0 * deg);
	// G4ThreeVector pos_test(0 * mm, 0 * mm, 0 * mm);
	// new G4PVPlacement(rot_test, pos_test, test, "test", mWorldLogical, false, 0, true);

	std::map<std::pair<int, int>, G4VSolid*> collimatorMap;
	// 초기화
	// collimatorMap[{20, 30}] = collimator_20_30;
	// collimatorMap[{20, 40}] = collimator_20_40;
	// collimatorMap[{20, 50}] = collimator_20_50;
	collimatorMap[{30, 30}] = collimator_30_30;
	collimatorMap[{30, 40}] = collimator_30_40;
	// collimatorMap[{30, 50}] = collimator_30_50;

	int bigSourceAperture = 30;
	int SmallSourceAperture = 30;

	G4VSolid* solid_collimator = collimatorMap[{bigSourceAperture, SmallSourceAperture}];

	// 로직 볼륨
	// 3D printer 필라멘트 (PLA)
	G4Element* el_C = new G4Element("Carbon", "C", 6, C_mass);
	G4Element* el_H = new G4Element("Hydrogen", "H", 1, H_mass);
	G4Element* el_O = new G4Element("Oxygen", "O", 8, O_mass);

	G4Material* ma_PLA = new G4Material("PLA", 1.25 * g/cm3, 3);

	ma_PLA->AddElement(el_C, 3);
	ma_PLA->AddElement(el_H, 4);
	ma_PLA->AddElement(el_O, 2);


	G4Material* ma_Al = mNist->FindOrBuildMaterial("G4_Al");
	G4String name = "logic_" + std::to_string(bigSourceAperture) + "_" + std::to_string(SmallSourceAperture);
	
	// 이렇게 Logical Volume을 정의하면 클래스 멤버와 무관한 지역 변수를 선언하는 것이어서 TAnalysisManager에서 불러오는 mCollimatorLogical에는 여전히 mullptr이 들어가게 됨..
	// G4LogicalVolume* mCollimatorLogical = new G4LogicalVolume(solid_collimator, ma_PLA, name);
	mCollimatorLogical = new G4LogicalVolume(solid_collimator, ma_PLA, name);

	G4RotationMatrix* rot_collimator = new G4RotationMatrix(0 * deg, 0 * deg, 0 * deg);
	G4ThreeVector pos_collimator(0 * mm, 0 * mm, 0 * mm);
	new G4PVPlacement(rot_collimator, pos_collimator, mCollimatorLogical, "Collimator", mWorldLogical, false, 0, true);



	// 쉴드 설치 및 제거


	// 한 솔리드에서 다른 솔리드를 뺄 때 사용한다.
	// 빼는 도형의 위치와 회전을 결정할 수 있음.


	// examples //
	// G4SubtractionSolid* subtractionSolid = new G4SubtractionSolid("SubtractionSolid", solidBox, solidTubs, 0, G4ThreeVector(0, 0, 0));
	// G4IntersectionSolid* intersectionSolid = new G4IntersectionSolid("IntersectionSolid", solidBox, solidTubs, 0, G4ThreeVector(0, 0, 0));
	// G4UnionSolid* unionSolid = new G4UnionSolid("UnionSolid", solidBox, solidTubs, 0, G4ThreeVector(0, 0, 0));

	// 논리 볼륨을 만든다. 매개변수는 솔리드 볼륨, 물질, 이름이다.
	// 알루미늄을 불러온다.
	// G4Material* Al = mNist->FindOrBuildMaterial("G4_Al");
	// G4LogicalVolume* test = new G4LogicalVolume(SubBigAperture4SmallSource, Al, "test");


	// examples //
	// G4LogicalVolume* boxLogical = new G4LogicalVolume(solidBox, material, "SolidBox");
	// G4LogicalVolume* tubsLogical = new G4LogicalVolume(solidTubs, material, "SolidTubs");
	// G4LogicalVolume* unionLogical = new G4LogicalVolume(unionSolid, material, "UnionSolid");
	// G4LogicalVolume* subtractionLogical = new G4LogicalVolume(solid_BoxSubTubs1, material, "SubtractionSolid");
	// G4LogicalVolume* intersectionLogical = new G4LogicalVolume(intersectionSolid, material, "IntersectionSolid");


	// 물리 볼륨을 배치한다. 매개변수는 회전, 위치, 로직 볼륨, 이름, 어미 논리 볼륨, 다중 여부, 복제 개수, 겹침 여부이다.
	// 회전
	// G4RotationMatrix* rot_test = new G4RotationMatrix(0 * deg, 0 * deg, 0 * deg);

	// 위치
	// G4ThreeVector pos_test(0 * mm, 0 * mm, 0 * mm);

	// new G4PVPlacement(rot_test, pos_test, test, "test", mWorldLogical, false, 0, true);	
	

	// examples //
	// new G4PVPlacement(rotation, position, boxLogical, "BOX", mWorldLogical, false, 0, true);
	// new G4PVPlacement(rotation_tubs, position_tubs, tubsLogical, "TUBS", mWorldLogical, false, 0, true);
	// new G4PVPlacement(rotation, position, unionLogical, "UNION", mWorldLogical, false, 0, true);
	// new G4PVPlacement(rotation1, position1, subtractionLogical, "SUBTRACTION", mWorldLogical, false, 0, true);
	// new G4PVPlacement(rotation, position, intersectionLogical, "INTERSECTION", mWorldLogical, false, 0, true);

}

void TDetectorConstruction::getShield() {
	G4double InnerRadius = 0. * mm;
	G4double OuterRadius = 5 * mm;
	G4double Height = 1 * mm;
	G4double StartAngle = 0 * deg;
	G4double EndAngle = 360 * deg;

	// G4double InnerRadius = 0 * mm;
	// G4double Height_tubsSource = 3 * mm;
	// G4double StartAngle_tubsSources = 0 * deg;
	// G4double EndAngle_tubsSources = 360 * deg;

	G4Tubs* solidShield = new G4Tubs("Tubs", InnerRadius, OuterRadius, .5 * Height, StartAngle, EndAngle);

	// 로직 볼륨
	// 3D printer 필라멘트 (PLA)
	G4Element* el_C = new G4Element("Carbon", "C", 6, C_mass);
	G4Element* el_H = new G4Element("Hydrogen", "H", 1, H_mass);
	G4Element* el_O = new G4Element("Oxygen", "O", 8, O_mass);

	G4Material* ma_PLA = new G4Material("PLA", 1.25 * g/cm3, 3);

	ma_PLA->AddElement(el_C, 3);
	ma_PLA->AddElement(el_H, 4);
	ma_PLA->AddElement(el_O, 2);

	mShieldLogical = new G4LogicalVolume(solidShield,ma_PLA,"Shield");

}

void TDetectorConstruction::getDetector() {
	G4double detectorX = 30 * mm;
	G4double detectorY = 15 * mm;
	G4double detectorZ = 100 * um;

	G4Box* solidDetector = new G4Box("ALPIDESolid", .5 * detectorX, .5 * detectorY, .5 * detectorZ);

	G4Material* silicon = mNist->FindOrBuildMaterial("G4_Si");

	mDetectorLogical = new G4LogicalVolume(solidDetector, silicon, "ALPIDE");

	mDetectorLogical->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));

	G4RotationMatrix* rot_detector = new G4RotationMatrix(0 * deg, 90 * deg, 90 * deg);
	G4ThreeVector pos_detector(0 * mm, -(6.5 + 2 + 12)* mm, 0 * mm); // -8.5는 콜리메이터의 끝, 그 뒤는 콜리메이터와 ALPIDE사이의 거리

	mDetector = new G4PVPlacement(rot_detector, pos_detector, mDetectorLogical, "ALPIDE", mWorldLogical, false, 0, true);

	G4Region* ALPIDERegion = new G4Region("ALPIDERegion");
	mDetectorLogical->SetRegion(ALPIDERegion);
	ALPIDERegion->AddRootLogicalVolume(mDetectorLogical);
}
