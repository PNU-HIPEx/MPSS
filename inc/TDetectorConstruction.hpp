#ifndef __TDETECTORCONSTRUCTION__
#define __TDETECTORCONSTRUCTION__

#include "G4VUserDetectorConstruction.hh"

#include "TConfig.h"

class G4VPhysicalVolume;
class G4LogicalVolume;

class TDetectorConstruction : public G4VUserDetectorConstruction {
public:
	TDetectorConstruction() = default;
	TDetectorConstruction(const KEI::TConfigFile& config);
	~TDetectorConstruction() override = default;
private:
	G4VPhysicalVolume* mWorld = nullptr;
	G4VPhysicalVolume* mTungsten = nullptr;
	G4VPhysicalVolume* mGlass = nullptr;
	G4VPhysicalVolume* mDetector = nullptr;

	G4LogicalVolume* mWorldLogical = nullptr;
	G4LogicalVolume* mTungstenLogical = nullptr;
	G4LogicalVolume* mGlassLogical = nullptr;
	G4LogicalVolume* mDetectorLogical = nullptr;

public:
	G4VPhysicalVolume* Construct() override;

	void getWorld();
	void getTungsten();
	void getGlass();
	void getDetector();

	G4LogicalVolume* getWorldLogical() const { return mWorldLogical; }
	G4LogicalVolume* getTungstenLogical() const { return mTungstenLogical; }
	G4LogicalVolume* getGlassLogical() const { return mGlassLogical; }
	G4LogicalVolume* getDetectorLogical() const { return mDetectorLogical; }
};

#endif