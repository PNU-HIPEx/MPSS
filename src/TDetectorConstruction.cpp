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
#include "config.hpp"
#include "CADMesh.hh"
#include "G4TessellatedSolid.hh"

const G4double AIR_DENSITY = 1.2929e-03 * g / cm3;
const G4double N_DENSITY = 14.01 * g / mole;
const G4double O_DENSITY = 16.00 * g / mole;

const std::filesystem::path sourceDir = SOURCE_DIR;
const std::string collimatorPath = sourceDir / "config/collimator.stl";


TDetectorConstruction::TDetectorConstruction(const KEI::TConfigFile& config) : G4VUserDetectorConstruction() {
	mAirPressure = config.getConfig("ENVIRONMENT").getValue<double>("AIR_PRESSURE");
}

G4VPhysicalVolume* TDetectorConstruction::Construct() {
	mNist = G4NistManager::Instance();
	getWorld();
	getCollimator();
	getDetector();

	return mWorld;
}

void TDetectorConstruction::getWorld() {
	G4double worldX = 50 * mm;
	G4double worldY = 50 * mm;
	G4double worldZ = 100 * mm;

	G4Box* solidWorld = new G4Box("World", .5 * worldX, .5 * worldY, .5 * worldZ);

	G4Material* worldMaterial = new G4Material("worldMaterial", mAirPressure * AIR_DENSITY, 2);
	G4Element* elN = new G4Element("Nitrogen", "N", 7, N_DENSITY);
	G4Element* elO = new G4Element("Oxygen", "O", 8, O_DENSITY);
	worldMaterial->AddElement(elN, .7);
	worldMaterial->AddElement(elO, .3);

	mWorldLogical = new G4LogicalVolume(solidWorld, worldMaterial, "World");

	mWorld = new G4PVPlacement(0, G4ThreeVector(), mWorldLogical, "World", 0, false, 0, true);
}

void TDetectorConstruction::getCollimator() {
	std::cout << collimatorPath << std::endl;
	auto mesh = CADMesh::TessellatedMesh::FromSTL(collimatorPath);

	auto collimatorSolid = mesh->GetSolid();

	G4Material* plastic = mNist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

	mCollimatorLogical = new G4LogicalVolume(collimatorSolid, plastic, "Collimator");

	mCollimatorLogical->SetVisAttributes(G4VisAttributes(G4Colour::Gray()));

	mCollimator = new G4PVPlacement(0, G4ThreeVector(), mCollimatorLogical, "Collimator", mWorldLogical, false, 0, true);

	G4Region* collimatorRegion = new G4Region("CollimatorRegion");
	mCollimatorLogical->SetRegion(collimatorRegion);
	collimatorRegion->AddRootLogicalVolume(mCollimatorLogical);

}

void TDetectorConstruction::getDetector() {
	G4double detectorX = 30 * mm;
	G4double detectorY = 15 * mm;
	G4double detectorZ = 100 * um;

	G4Box* solidDetector = new G4Box("ALPIDESolid", .5 * detectorX, .5 * detectorY, .5 * detectorZ);

	G4Material* silicon = mNist->FindOrBuildMaterial("G4_Si");

	mDetectorLogical = new G4LogicalVolume(solidDetector, silicon, "ALPIDE");

	mDetectorLogical->SetVisAttributes(G4VisAttributes(G4Colour::Yellow()));

	mDetector = new G4PVPlacement(0, G4ThreeVector(), mDetectorLogical, "ALPIDE", mWorldLogical, false, 0, true);

	G4Region* ALPIDERegion = new G4Region("ALPIDERegion");
	mDetectorLogical->SetRegion(ALPIDERegion);
	ALPIDERegion->AddRootLogicalVolume(mDetectorLogical);
}