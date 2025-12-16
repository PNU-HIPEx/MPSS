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
	CASE_A,
	CASE_B,
	CASE_C,
	DETECTOR
};

int getVolumeID(G4LogicalVolume* volume) {
	const TDetectorConstruction* detectorConstruction = static_cast<const TDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());

	G4LogicalVolume* worldLogical = detectorConstruction->getWorldLogical();
	G4LogicalVolume* collimatorLogical = detectorConstruction->getCollimatorLogical();
	G4LogicalVolume* shieldVerticalLogical = detectorConstruction->getShieldVerticalLogical();
	G4LogicalVolume* shieldLeftLogical = detectorConstruction->getShieldLeftLogical();
	G4LogicalVolume* shieldRightLogical = detectorConstruction->getShieldRightLogical();
	G4LogicalVolume* caseVerticalLogical = detectorConstruction->getCaseVerticalLogical();
	G4LogicalVolume* caseLeftLogical = detectorConstruction->getCaseLeftLogical();
	G4LogicalVolume* caseRightLogical = detectorConstruction->getCaseRightLogical();
	G4LogicalVolume* detectorLogical = detectorConstruction->getDetectorLogical();


	if ( volume == worldLogical ) {
		return 0;
	} else if ( volume == collimatorLogical ) {
		return 1;
	} else if ( volume == shieldVerticalLogical ) {
		return 2;
	} else if ( volume == shieldLeftLogical ) {
		return 3;
	} else if ( volume == shieldRightLogical ) {
		return 4;
	} else if ( volume == caseVerticalLogical ) {
		return 5;
	} else if ( volume == caseLeftLogical ) {
		return 6;
	} else if ( volume == caseRightLogical ) {
		return 7;
	} else if ( volume == detectorLogical ) {
		return 8;
	} else {
		return 9;
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
		case TVolumeID::CASE_A:
			return "Upper Source Case";
			break;
		case TVolumeID::CASE_B:
			return "Left Source Case";
			break;
		case TVolumeID::CASE_C:
			return "Left Source Case";
			break;
	}
}

#endif