#ifndef __TVOLUMEID__
#define __TVOLUMEID__

#include "G4LogicalVolume.hh"
#include "G4RunManager.hh"
#include "TDetectorConstruction.hpp"

enum TVolumeID {
	WORLD,
	COLLIMATOR,
	SHEILD_A,
	SHEILD_B,
	SHEILD_C,
	DETECTOR
};

int getVolumeID(G4LogicalVolume* volume) {
	const TDetectorConstruction* detectorConstruction = static_cast<const TDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());

	G4LogicalVolume* worldLogical = detectorConstruction->getWorldLogical();
	G4LogicalVolume* collimatorLogical = detectorConstruction->getCollimatorLogical();
	G4LogicalVolume* detectorLogical = detectorConstruction->getDetectorLogical();

	if ( volume == worldLogical ) {
		return 0;
	} else if ( volume == collimatorLogical ) {
		return 1;
	} else if ( volume == detectorLogical ) {
		return 5;
	} else {
		return 2;
	}
}

std::string getVolumeName(TVolumeID volumeID) {
	switch ( volumeID ) {
		case TVolumeID::WORLD:
			return "World";
			break;
		case TVolumeID::COLLIMATOR:
			return "Collimator";
			break;
		case TVolumeID::DETECTOR:
			return "ALPIDE";
			break;
		case TVolumeID::SHEILD_A:
			return "Upper Sheild";
			break;
		case TVolumeID::SHEILD_B:
			return "Left Sheild";
			break;
		case TVolumeID::SHEILD_C:
			return "Right Sheild";
			break;

	}
}

#endif