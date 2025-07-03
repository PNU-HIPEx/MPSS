#include "TDetectorConstruction.hpp"

#include "G4SystemOfUnits.hh"
#include "G4Box.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"

const G4double AIR_DENSITY = 1.2929e-03 * g / cm3;
const G4double N_DENSITY = 14.01 * g / mole;
const G4double O_DENSITY = 16.00 * g / mole;

TDetectorConstruction::TDetectorConstruction(const KEI::TConfigFile& config) : G4VUserDetectorConstruction() {
	mTungstenPosition = config.getConfig("ENVIRONMENT").getValue<double, 3>("TUNGSTEN_POSITION");
	mTungstenAngle = config.getConfig("ENVIRONMENT").getValue<double, 3>("TUNGSTEN_ANGLE");
}

G4VPhysicalVolume* TDetectorConstruction::Construct() {
	getWorld();
	getTungsten();
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

void TDetectorConstruction::getTungsten() {
	G4double tungstenX = 5 * mm;
	G4double tungstenY = 5 * mm;
	G4double tungstenZ = 1 * mm;

	G4Box* solidTungsten = new G4Box("Tungsten", .5 * tungstenX, .5 * tungstenY, .5 * tungstenZ);

	G4NistManager* nist = G4NistManager::Instance();
	G4Material* tungstenMaterial = nist->FindOrBuildMaterial("G4_W");

	if ( !tungstenMaterial ) {
		G4Exception("TDetectorConstruction::getTungsten",
					"MyCode001", FatalException,
					"Tungsten material (G4_W) not found!");
	}

	mTungstenLogical = new G4LogicalVolume(solidTungsten, tungstenMaterial, "Tungsten");

	G4RotationMatrix* rotation = new G4RotationMatrix(mTungstenAngle[0] * deg, mTungstenAngle[1] * deg, mTungstenAngle[2] * deg);
	G4ThreeVector position(mTungstenPosition[0] * mm, mTungstenPosition[1] * mm, mTungstenPosition[2] * mm);
	new G4PVPlacement(rotation, position, mTungstenLogical, "Tungsten", mWorldLogical, false, 0, true);

	G4Region* tungstenRegion = new G4Region("TungstenRegion");
	mTungstenLogical->SetRegion(tungstenRegion);
	tungstenRegion->AddRootLogicalVolume(mTungstenLogical);
}