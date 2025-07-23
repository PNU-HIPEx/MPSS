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
#include "G4NistManager.hh"

const G4double AIR_DENSITY = 1.2929e-03 * g / cm3;
const G4double N_DENSITY = 14.01 * g / mole;
const G4double O_DENSITY = 16.00 * g / mole;

TDetectorConstruction::TDetectorConstruction(const KEI::TConfigFile& config) : G4VUserDetectorConstruction() { }

G4VPhysicalVolume* TDetectorConstruction::Construct() {
	mNist = G4NistManager::Instance();
	getWorld();
	getCollimator();
	// getGlass();
	// getDetector();

	return mWorld;
}

void TDetectorConstruction::getWorld() {
	G4double worldX = 50 * mm;
	G4double worldY = 50 * mm;
	G4double worldZ = 100 * mm;

	G4Box* solidWorld = new G4Box("World", .5 * worldX, .5 * worldY, .5 * worldZ);

	G4Material* worldMaterial = new G4Material("worldMaterial", 0.001 * AIR_DENSITY, 2);
	G4Element* elN = new G4Element("Nitrogen", "N", 7, N_DENSITY);
	G4Element* elO = new G4Element("Oxygen", "O", 8, O_DENSITY);
	worldMaterial->AddElement(elN, .7);
	worldMaterial->AddElement(elO, .3);

	mWorldLogical = new G4LogicalVolume(solidWorld, worldMaterial, "World");

	mWorld = new G4PVPlacement(0, G4ThreeVector(), mWorldLogical, "World", 0, false, 0, true);
}

void TDetectorConstruction::getCollimator() {
	/*
		GEANT4에서 사물을 배치하는 방법
		1. 사물의 형태인 솔리드 볼륨 먼저 제작한다.
		2. 앞서 제작한 형태에 재질을 추가한 논리 볼륨을 만든다.
		3. 논리 볼륨에 위치와 방향을 지정하여 물리 볼륨을 배치한다.
	*/

	// 형태 제작 튜토리얼
	// G4Box
	G4double boxX = 10 * mm;
	G4double boxY = 15 * mm;
	G4double boxZ = 5 * mm;
	// 박스를 제작한다. 매개변수는 이름, x 크기의 절반, y 크기의 절반, z 크기의 절반이다.
	G4Box* solidBox = new G4Box("Box", .5 * boxX, .5 * boxY, .5 * boxZ);

	// G4Tubs
	G4double tubsInnerRadius = 5 * mm;
	G4double tubsOuterRadius = 10 * mm;
	G4double tubsHeight = 3 * mm;
	G4double tubsStartPhiAngle = 45 * deg;
	G4double tubsAngle = 90 * deg;

	// 다라이를 제작한다. 매개변수는 이름, 내경, 외경, 높이의 절반, 시작 각도, 총 각도이다.
	G4Tubs* solidTubs = new G4Tubs("Tubs", tubsInnerRadius, tubsOuterRadius, .5 * tubsHeight, tubsStartPhiAngle, tubsAngle);

	// 두 솔리드를 합칠 때 사용한다.
	G4UnionSolid* unionSolid = new G4UnionSolid("UnionSolid", solidBox, solidTubs, 0, G4ThreeVector(0, 0, 0));

	// 한 솔리드에서 다른 솔리드를 뺄 때 사용한다.
	G4SubtractionSolid* subtractionSolid = new G4SubtractionSolid("SubtractionSolid", solidBox, solidTubs, 0, G4ThreeVector(0, 0, 0));

	// 두 솔리드의 겹치는 부분을 사용한다.
	G4IntersectionSolid* intersectionSolid = new G4IntersectionSolid("IntersectionSolid", solidBox, solidTubs, 0, G4ThreeVector(0, 0, 0));

	// 알루미늄을 불러온다.
	G4Material* material = mNist->FindOrBuildMaterial("G4_Al");
	// 논리 볼륨을 만든다. 매개변수는 솔리드 볼륨, 물질, 이름이다.
	G4LogicalVolume* boxLogical = new G4LogicalVolume(solidBox, material, "SolidBox");
	G4LogicalVolume* tubsLogical = new G4LogicalVolume(solidTubs, material, "SolidTubs");
	G4LogicalVolume* unionLogical = new G4LogicalVolume(unionSolid, material, "UnionSolid");
	G4LogicalVolume* subtractionLogical = new G4LogicalVolume(subtractionSolid, material, "SubtractionSolid");
	G4LogicalVolume* intersectionLogical = new G4LogicalVolume(intersectionSolid, material, "IntersectionSolid");

	// 회전과 위치를 지정한다.
	G4RotationMatrix* rotation = new G4RotationMatrix(0 * deg, 0 * deg, 0 * deg);
	G4ThreeVector position(0 * mm, 0 * mm, 0 * mm);
	// 물리 볼륨을 배치한다. 매개변수는 회전, 위치, 로직 볼륨, 이름, 어미 논리 볼륨, 다중 여부, 복제 개수, 겹침 여부이다.
	// new G4PVPlacement(rotation, position, boxLogical, "BOX", mWorldLogical, false, 0, true);
	// new G4PVPlacement(rotation, position, tubsLogical, "TUBS", mWorldLogical, false, 0, true);
	// new G4PVPlacement(rotation, position, unionLogical, "UNION", mWorldLogical, false, 0, true);
	// new G4PVPlacement(rotation, position, subtractionLogical, "SUBTRACTION", mWorldLogical, false, 0, true);
	// new G4PVPlacement(rotation, position, intersectionLogical, "INTERSECTION", mWorldLogical, false, 0, true);
}

void TDetectorConstruction::getDetector() {
	G4double detectorX = 30 * mm;
	G4double detectorY = 15 * mm;
	G4double detectorZ = 100 * um;

	G4Box* solidDetector = new G4Box("ALPIDESolid", .5 * detectorX, .5 * detectorY, .5 * detectorZ);

	G4Material* silicon = mNist->FindOrBuildMaterial("G4_Si");

	mDetectorLogical = new G4LogicalVolume(solidDetector, silicon, "ALPIDE");

	mDetector = new G4PVPlacement(0, G4ThreeVector(), mDetectorLogical, "ALPIDE", mDetectorLogical, false, 0, true);

	G4Region* ALPIDERegion = new G4Region("ALPIDERegion");
	mDetectorLogical->SetRegion(ALPIDERegion);
	ALPIDERegion->AddRootLogicalVolume(mDetectorLogical);
}