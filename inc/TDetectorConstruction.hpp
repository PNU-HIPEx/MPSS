#ifndef __TDETECTORCONSTRUCTION__
#define __TDETECTORCONSTRUCTION__

#include "G4VUserDetectorConstruction.hh"
#include "G4NistManager.hh"

#include "TConfig.hpp"

class G4VPhysicalVolume;
class G4LogicalVolume;

class TDetectorConstruction : public G4VUserDetectorConstruction {
public:
	TDetectorConstruction() = default;
	TDetectorConstruction(const KEI::TConfigFile& config);
	~TDetectorConstruction() override = default;
private:
	G4NistManager* mNist;

	G4VPhysicalVolume* mWorld = nullptr;
	G4VPhysicalVolume* mCollimator = nullptr;
	G4VPhysicalVolume* mDetector = nullptr;

	G4LogicalVolume* mWorldLogical = nullptr;
	G4LogicalVolume* mCollimatorLogical = nullptr;
	G4LogicalVolume* mDetectorLogical = nullptr;

public:
	G4VPhysicalVolume* Construct() override;

	void getWorld();
	void getCollimator();
	void getDetector();

	G4LogicalVolume* getWorldLogical() const { return mWorldLogical; }
	G4LogicalVolume* getCollimatorLogical() const { return mCollimatorLogical; }
	G4LogicalVolume* getDetectorLogical() const { return mDetectorLogical; }
};

#endif