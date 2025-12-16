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
#include "CADMesh.hh"
#include "config.hpp"

const G4double AIR_DENSITY = 1.2929e-03 * g / cm3;
const G4double N_mass = 14.01 * g / mole;
const G4double O_mass = 16.00 * g / mole;

const G4double C_mass = 12.01 * g / mole;
const G4double H_mass = 1.008 * g / mole;

const std::filesystem::path sourceDir = SOURCE_DIR;

TDetectorConstruction::TDetectorConstruction(const KEI::TConfigFile& config) : G4VUserDetectorConstruction(), mConfig(config) {
	mAirPressure = config.getConfig("ENVIRONMENT").hasKey("AIR_PRESSURE") ? config.getConfig("ENVIRONMENT").getValue<double>("AIR_PRESSURE") : 1.;
	mCollimatorSide = config.getConfig("ENVIRONMENT").hasKey("COLLIMATOR_SIDE") ? config.getConfig("ENVIRONMENT").getValue<double>("COLLIMATOR_SIDE") : 4.;
	mCollimatorUpper = config.getConfig("ENVIRONMENT").hasKey("COLLIMATOR_UPPER") ? config.getConfig("ENVIRONMENT").getValue<double>("COLLIMATOR_UPPER") : 3.;
}

void TDetectorConstruction::getCollimatorMaterial() {
	std::string materialName = mConfig.getConfig("ENVIRONMENT").hasKey("COLLIMATOR_MATERIAL") ? mConfig.getConfig("ENVIRONMENT").getValue<std::string>("COLLIMATOR_MATERIAL") : "PLA";
	std::cout << materialName;
	if ( materialName == "PLA" ) {
		G4Element* el_C = new G4Element("Carbon", "C", 6, C_mass);
		G4Element* el_H = new G4Element("Hydrogen", "H", 1, H_mass);
		G4Element* el_O = new G4Element("Oxygen", "O", 8, O_mass);
		mCollimatorMaterial = new G4Material("PLA", 1.25 * g / cm3, 3);
		mCollimatorMaterial->AddElement(el_C, 3);
		mCollimatorMaterial->AddElement(el_H, 4);
		mCollimatorMaterial->AddElement(el_O, 2);
	} else if ( materialName == "AL" ) {
		mCollimatorMaterial = mNist->FindOrBuildMaterial("G4_Al");
	} else {
		G4cerr << "Unknown collimator material: " << materialName << "." << G4endl;
		exit(1);
	}
}

G4VPhysicalVolume* TDetectorConstruction::Construct() {
	mNist = G4NistManager::Instance();
	getCollimatorMaterial();
	getWorld();
	getCollimator();
	getShield();
	getDetector();

	return mWorld;
}

void TDetectorConstruction::getWorld() {
	G4double worldX = 50 * mm;
	G4double worldY = 70 * mm;
	G4double worldZ = 50 * mm;

	G4Box* solidWorld = new G4Box("World", .5 * worldX, .5 * worldY, .5 * worldZ);

	G4Material* worldMaterial = new G4Material("worldMaterial", mAirPressure * AIR_DENSITY, 2);
	G4Element* elN = new G4Element("Nitrogen", "N", 7, N_mass);
	G4Element* elO = new G4Element("Oxygen", "O", 8, O_mass);
	worldMaterial->AddElement(elN, .7);
	worldMaterial->AddElement(elO, .3);

	mWorldLogical = new G4LogicalVolume(solidWorld, worldMaterial, "World");

	mWorld = new G4PVPlacement(0, G4ThreeVector(), mWorldLogical, "World", 0, false, 0, true);
}

void TDetectorConstruction::getCollimator() {
	std::string collimatorPath = sourceDir / "config/collimator.stl";

	auto mesh = CADMesh::TessellatedMesh::FromSTL(collimatorPath);
	G4VSolid* meshSolid = mesh->GetSolid();
	G4ThreeVector position(0 * mm, 0 * mm, -8.38 * mm);
	G4VSolid* bodySolid = new G4DisplacedSolid("bodySolid", meshSolid, nullptr, position);

	G4Tubs* sideHole = new G4Tubs("sideHole", 0 * mm, (mCollimatorSide / 2.) * mm, 10 * mm, 0 * deg, 360 * deg);
	G4RotationMatrix* sideRotation = new G4RotationMatrix();
	sideRotation->rotateY(90 * deg);

	G4SubtractionSolid* collimatorSideHoleSolid = new G4SubtractionSolid("collimator_with_side_hole", bodySolid, sideHole, sideRotation, G4ThreeVector());

	G4Tubs* verticalHole = new G4Tubs("sideHole", 0 * mm, (mCollimatorUpper / 2.) * mm, 10 * mm, 0 * deg, 360 * deg);
	G4SubtractionSolid* collimatorSolid = new G4SubtractionSolid("collimator", collimatorSideHoleSolid, verticalHole);

	mCollimatorLogical = new G4LogicalVolume(collimatorSolid, mCollimatorMaterial, "Collimator");

	new G4PVPlacement(nullptr, G4ThreeVector(), mCollimatorLogical, "Collimator", mWorldLogical, false, 0, true);

	G4Region* collimatorRegion = new G4Region("CollimatorRegion");
	mCollimatorLogical->SetRegion(collimatorRegion);
	collimatorRegion->AddRootLogicalVolume(mCollimatorLogical);
}

void TDetectorConstruction::getShield() {
	if ( mConfig.getConfig("ENVIRONMENT").hasKey("SHIELD_VERTICAL") && mConfig.getConfig("ENVIRONMENT").getValue<bool>("SHIELD_VERTICAL") ) {
		std::string meshPath = sourceDir / "config/shield_vertical.stl";
		auto mesh = CADMesh::TessellatedMesh::FromSTL(meshPath);
		G4VSolid* meshSolid = mesh->GetSolid();
		mShieldVerticalLogical = new G4LogicalVolume(meshSolid, mCollimatorMaterial, "ShieldVertical");
		G4RotationMatrix* rotation = new G4RotationMatrix();
		rotation->rotateZ(-90 * deg);
		G4ThreeVector position(0 * mm, -8.6 * mm, 6.62 * mm);
		new G4PVPlacement(rotation, position, mShieldVerticalLogical, "ShieldVertical", mWorldLogical, false, 0, true);
	}
	if ( mConfig.getConfig("ENVIRONMENT").hasKey("SHIELD_LEFT") && mConfig.getConfig("ENVIRONMENT").getValue<bool>("SHIELD_LEFT") ) {
		std::string meshPath = sourceDir / "config/shield_side.stl";
		auto mesh = CADMesh::TessellatedMesh::FromSTL(meshPath);
		G4VSolid* meshSolid = mesh->GetSolid();
		mShieldLeftLogical = new G4LogicalVolume(meshSolid, mCollimatorMaterial, "ShieldLeft");
		G4RotationMatrix* rotation = new G4RotationMatrix();
		rotation->rotateZ(-90 * deg);
		rotation->rotateX(90 * deg);
		G4ThreeVector position(-4.5 * mm, -11.2 * mm, 0 * mm);
		new G4PVPlacement(rotation, position, mShieldLeftLogical, "ShieldLeft", mWorldLogical, false, 0, true);
	}
	if ( mConfig.getConfig("ENVIRONMENT").hasKey("SHIELD_RIGHT") && mConfig.getConfig("ENVIRONMENT").getValue<bool>("SHIELD_RIGHT") ) {
		std::string meshPath = sourceDir / "config/shield_side.stl";
		auto mesh = CADMesh::TessellatedMesh::FromSTL(meshPath);
		G4VSolid* meshSolid = mesh->GetSolid();
		mShieldRightLogical = new G4LogicalVolume(meshSolid, mCollimatorMaterial, "ShieldRight");
		G4RotationMatrix* rotation = new G4RotationMatrix();
		rotation->rotateZ(-90 * deg);
		rotation->rotateX(-90 * deg);
		G4ThreeVector position(4.5 * mm, -11.2 * mm, 0 * mm);
		new G4PVPlacement(rotation, position, mShieldRightLogical, "ShieldRight", mWorldLogical, false, 0, true);
	}
}

void TDetectorConstruction::getDetector() {
	G4double detectorX = 30 * mm;
	G4double detectorY = 15 * mm;
	G4double detectorZ = 100 * um;

	G4Box* solidDetector = new G4Box("ALPIDESolid", .5 * detectorX, .5 * detectorY, .5 * detectorZ);

	G4Material* silicon = mNist->FindOrBuildMaterial("G4_Si");

	mDetectorLogical = new G4LogicalVolume(solidDetector, silicon, "ALPIDE");

	mDetectorLogical->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));

	G4ThreeVector pos_detector(0 * mm, 0 * mm, -(6.5 + 2 + 12) * mm);

	mDetector = new G4PVPlacement(nullptr, pos_detector, mDetectorLogical, "ALPIDE", mWorldLogical, false, 0, true);

	G4Region* ALPIDERegion = new G4Region("ALPIDERegion");
	mDetectorLogical->SetRegion(ALPIDERegion);
	ALPIDERegion->AddRootLogicalVolume(mDetectorLogical);
}
