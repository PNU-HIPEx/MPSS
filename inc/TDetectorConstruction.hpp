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
	KEI::TConfigFile mConfig;
	G4NistManager* mNist;

	G4double mAirPressure = 1.;
	G4Material* mCollimatorMaterial;
	G4double mCollimatorSide = 4.;
	G4double mCollimatorUpper = 3.;

	G4VPhysicalVolume* mWorld = nullptr;
	G4VPhysicalVolume* mCollimator = nullptr;
	G4VPhysicalVolume* mShieldVertical = nullptr;
	G4VPhysicalVolume* mShieldLeft = nullptr;
	G4VPhysicalVolume* mShieldRight = nullptr;
	G4VPhysicalVolume* mCaseVertical = nullptr;
	G4VPhysicalVolume* mCaseLeft = nullptr;
	G4VPhysicalVolume* mCaseRight = nullptr;
	G4VPhysicalVolume* mDetector = nullptr;

	G4LogicalVolume* mWorldLogical = nullptr;
	G4LogicalVolume* mCollimatorLogical = nullptr;
	G4LogicalVolume* mShieldVerticalLogical = nullptr;
	G4LogicalVolume* mShieldLeftLogical = nullptr;
	G4LogicalVolume* mShieldRightLogical = nullptr;
	G4LogicalVolume* mCaseVerticalLogical = nullptr;
	G4LogicalVolume* mCaseLeftLogical = nullptr;
	G4LogicalVolume* mCaseRightLogical = nullptr;
	G4LogicalVolume* mDetectorLogical = nullptr;

public:
	G4VPhysicalVolume* Construct() override;

	void getWorld();
	void getCollimator();
	void getShield();
	void getDetector();
	void getCollimatorMaterial();
	void getCase();

	G4LogicalVolume* getWorldLogical() const { return mWorldLogical; }
	G4LogicalVolume* getCollimatorLogical() const { return mCollimatorLogical; }
	G4LogicalVolume* getShieldVerticalLogical() const { return mShieldVerticalLogical; }
	G4LogicalVolume* getShieldLeftLogical() const { return mShieldLeftLogical; }
	G4LogicalVolume* getShieldRightLogical() const { return mShieldRightLogical; }
	G4LogicalVolume* getCaseVerticalLogical() const { return mCaseVerticalLogical; }
	G4LogicalVolume* getCaseLeftLogical() const { return mCaseLeftLogical; }
	G4LogicalVolume* getCaseRightLogical() const { return mCaseRightLogical; }
	G4LogicalVolume* getDetectorLogical() const { return mDetectorLogical; }
};

#endif