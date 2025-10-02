#include "TAnalysisManager.hpp"

#include<filesystem>

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4Step.hh"

#include "TFile.h"
#include "TTree.h"

#include "TDetectorConstruction.hpp"

#include "cpptqdm.h"


TAnalysisManager* TAnalysisManager::mInstance = nullptr;

TAnalysisManager::TAnalysisManager() {
	mInstance = this;
}

TAnalysisManager::TAnalysisManager(const KEI::TConfigFile& config) : mConfig(config) {
	mInstance = this;
	open();
}

TAnalysisManager::~TAnalysisManager() {
	close();
}

TAnalysisManager* TAnalysisManager::Instance() {
	if ( mInstance == 0 ) {
		mInstance = new TAnalysisManager();
	}
	return mInstance;
}

void TAnalysisManager::open() {
	TString outputPath = mConfig.getConfig("FILE").getValue<std::string>("OUTPUT_FILE");

	mFile = new TFile(outputPath, "RECREATE");
	mTrackTree = new TTree("trackTree", "Track Information");

	// values initial
	mTrackTree->Branch("eventID", &mTrackTuple.eventID);
	mTrackTree->Branch("trackID", &mTrackTuple.trackID);
	mTrackTree->Branch("parentID", &mTrackTuple.parentID);
	mTrackTree->Branch("particleID", &mTrackTuple.particleID);
	mTrackTree->Branch("initX", &mTrackTuple.initX);
	mTrackTree->Branch("initY", &mTrackTuple.initY);
	mTrackTree->Branch("initZ", &mTrackTuple.initZ);
	mTrackTree->Branch("initPX", &mTrackTuple.initPX);
	mTrackTree->Branch("initPY", &mTrackTuple.initPY);
	mTrackTree->Branch("initPZ", &mTrackTuple.initPZ);

	// values incident
	mTrackTree->Branch("isInALPIDE", &mTrackTuple.isInALPIDE);
	mTrackTree->Branch("initKineticEnergy", &mTrackTuple.initKineticEnergy);
	mTrackTree->Branch("initVolumeID", &mTrackTuple.initVolumeID);
	mTrackTree->Branch("inciX", &mTrackTuple.inciX);
	mTrackTree->Branch("inciY", &mTrackTuple.inciY);
	mTrackTree->Branch("inciZ", &mTrackTuple.inciZ);
	mTrackTree->Branch("inciPX", &mTrackTuple.inciPX);
	mTrackTree->Branch("inciPY", &mTrackTuple.inciPY);
	mTrackTree->Branch("inciPZ", &mTrackTuple.inciPZ);
	mTrackTree->Branch("inciKineticEnergy", &mTrackTuple.initKineticEnergy);
	mTrackTree->Branch("inciVolumeID", &mTrackTuple.initVolumeID);

	// values final
	mTrackTree->Branch("finalX", &mTrackTuple.finalX);
	mTrackTree->Branch("finalY", &mTrackTuple.finalY);
	mTrackTree->Branch("finalZ", &mTrackTuple.finalZ);
	mTrackTree->Branch("finalPX", &mTrackTuple.finalPX);
	mTrackTree->Branch("finalPY", &mTrackTuple.finalPY);
	mTrackTree->Branch("finalPZ", &mTrackTuple.finalPZ);
	mTrackTree->Branch("finalKineticEnergy", &mTrackTuple.finalKineticEnergy);
	mTrackTree->Branch("finalVolumeID", &mTrackTuple.finalVolumeID);

}

Int_t TAnalysisManager::getParticleID(const G4String& particleID) {
	if ( particleID == "alpha" ) {
		return PARTICLE::alpha;
	} else if ( particleID == "e-" ) {
		return PARTICLE::electron;
	} else if ( particleID == "gamma" ) {
		return PARTICLE::gammaRay;
	} else if ( particleID == "proton" ) {
		return PARTICLE::proton;
	} else if ( particleID == "neutron" ) {
		return PARTICLE::neutron;
	} else {
		return PARTICLE::unknown;
	}
}

// Int_t TAnalysisManager::getVolumeID(const G4LogicalVolume* volume) {
// 	if ( volume == mWorldLogical ) {
// 		return 1;
// 	} else if ( volume == mTungstenLogical ) {
// 		return 2;
// 	} else if ( volume == mGlassLogical ) {
// 		return 3;
// 	} else if ( volume == mDetectorLogical ) {
// 		return 4;
// 	} else {
// 		return 0;
// 	}
// }

Int_t TAnalysisManager::getVolumeID(const G4LogicalVolume* volume) {
	if ( volume == mWorldLogical ) 
	{
		return 1;
	} 
	else if ( volume == mDetectorLogical ) 
	{
		return 2;
	} 
	else if ( volume == mCollimatorLogical ) 
	{
		return 3;
	}
	else if ( volume == mShieldLogical )
	{
		return 4;
	} 
	else 
	{
		return 0;
	}
}



void TAnalysisManager::close() {
	mFile->cd();
	mTrackTree->Write();
	mFile->Close();
}

void TAnalysisManager::doBeginOfRun(const G4Run* run) {
	Int_t nEvent = run->GetNumberOfEventToBeProcessed();
	mProgressBar = new ProgressBar(nEvent);
}

void TAnalysisManager::doEndOfRun(const G4Run* run) {
	delete mProgressBar;
	mProgressBar = nullptr;

	std::cout << "Unknown particles: ";
	for ( const std::string& particle : mUnknownParticleList ) {
		std::cout << particle << " ";
	}
	std::cout << std::endl;
}

void TAnalysisManager::doBeginOfEvent(const G4Event* event) {
	mProgressBar->countUp();
	mTrackTuple.eventID = event->GetEventID();
}

void TAnalysisManager::doEndOfEvent(const G4Event* event) { }

void TAnalysisManager::doPreTracking(const G4Track* track) {
	Int_t eventID = mTrackTuple.eventID;
	mTrackTuple.init();

	mTrackTuple.eventID = eventID;
	mTrackTuple.trackID = track->GetTrackID();
	mTrackTuple.parentID = track->GetParentID();
	mTrackTuple.particleID = getParticleID(track->GetDefinition()->GetParticleName());

	// 발생한 입자가 알파, 중성자, 감마, 전자, 양전자, 양성자가 아닐 때
	if ( mTrackTuple.particleID == 0 ) {
		if ( std::find(mUnknownParticleList.begin(), mUnknownParticleList.end(), track->GetDefinition()->GetParticleName()) == mUnknownParticleList.end() ) {
			mUnknownParticleList.push_back(track->GetDefinition()->GetParticleName());
		}
	}
	isInALPIDE = false;
}

void TAnalysisManager::doPostTracking(const G4Track* track) {
	const TDetectorConstruction* detectorConstruction = static_cast<const TDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	if ( mWorldLogical == nullptr ) {
		mWorldLogical = detectorConstruction->getWorldLogical();
	}
	if ( mCollimatorLogical == nullptr ) {
		mCollimatorLogical = detectorConstruction->getCollimatorLogical();
	}

	// values_initial
	G4ThreeVector vertexPosition = track->GetVertexPosition();
	mTrackTuple.initX = vertexPosition.x();
	mTrackTuple.initY = vertexPosition.y();
	mTrackTuple.initZ = vertexPosition.z();

	G4ThreeVector vertexMomentum = track->GetVertexMomentumDirection();
	mTrackTuple.initPX = vertexMomentum.x();
	mTrackTuple.initPY = vertexMomentum.y();
	mTrackTuple.initPZ = vertexMomentum.z();

	mTrackTuple.initKineticEnergy = track->GetVertexKineticEnergy();
	mTrackTuple.initVolumeID = getVolumeID(track->GetOriginTouchableHandle()->GetVolume()->GetLogicalVolume());

	// values_final
	G4ThreeVector finalPosition = track->GetPosition();
	mTrackTuple.finalX = finalPosition.x();
	mTrackTuple.finalY = finalPosition.y();
	mTrackTuple.finalZ = finalPosition.z();

	G4ThreeVector finalMomentum = track->GetMomentumDirection();
	mTrackTuple.finalPX = finalMomentum.x();
	mTrackTuple.finalPY = finalMomentum.y();
	mTrackTuple.finalPZ = finalMomentum.z();

	mTrackTuple.finalKineticEnergy = track->GetKineticEnergy();

	mTrackTuple.finalVolumeID = getVolumeID(track->GetVolume()->GetLogicalVolume());

	mTrackTree->Fill();
}

void TAnalysisManager::doStepPhase(const G4Step* step) {
	G4LogicalVolume* preVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
	G4int volumeID = getVolumeID(preVolume);

	const TDetectorConstruction* detectorConstruction = static_cast<const TDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	if ( mWorldLogical == nullptr ) {
		mWorldLogical = detectorConstruction->getWorldLogical();
	}
	if ( mDetectorLogical == nullptr ) {
		mDetectorLogical = detectorConstruction->getDetectorLogical();
	}
	if ( mCollimatorLogical == nullptr ) {
		mCollimatorLogical = detectorConstruction->getCollimatorLogical();
	}
	// Detector에서 생성되는 secondary에 대한 고려가 필요함.
	if ( !isInALPIDE && preVolume == mDetectorLogical && step->IsFirstStepInVolume() ) {
		// values_incident
		isInALPIDE = true;
		mTrackTuple.isInALPIDE = true;
		mTrackTuple.inciX = step->GetPreStepPoint()->GetPosition().x();
		mTrackTuple.inciY = step->GetPreStepPoint()->GetPosition().y();
		mTrackTuple.inciZ = step->GetPreStepPoint()->GetPosition().z();
		mTrackTuple.inciPX = step->GetPreStepPoint()->GetMomentum().x();
		mTrackTuple.inciPY = step->GetPreStepPoint()->GetMomentum().y();
		mTrackTuple.inciPZ = step->GetPreStepPoint()->GetMomentum().z();
		mTrackTuple.inciKineticEnergy = step->GetPreStepPoint()->GetKineticEnergy();
		mTrackTuple.inciVolumeID = getVolumeID(preVolume);
		// mTrackTuple.globalTime = step->GetPreStepPoint()->GetGlobalTime();
		// mTrackTuple.localTime = step->GetPreStepPoint()->GetLocalTime();
	}



	// if ( !isInALPIDE && currentVolume == mDetectorLogical && step->IsFirstStepInVolume() ) {
	// 	isInALPIDE = true;
	// 	mTrackTuple.inciX = step->GetPreStepPoint()->GetPosition().x();
	// 	mTrackTuple.inciY = step->GetPreStepPoint()->GetPosition().y();
	// 	mTrackTuple.inciZ = step->GetPreStepPoint()->GetPosition().z();
	// 	mTrackTuple.inciPX = step->GetPreStepPoint()->GetMomentum().x();
	// 	mTrackTuple.inciPY = step->GetPreStepPoint()->GetMomentum().y();
	// 	mTrackTuple.inciPZ = step->GetPreStepPoint()->GetMomentum().z();
	// 	mTrackTuple.inciKineticEnergy = step->GetPreStepPoint()->GetKineticEnergy();
	// 	mTrackTuple.inciVolumeID = getVolumeID(currentVolume);
	// 	// mTrackTuple.globalTime = step->GetPreStepPoint()->GetGlobalTime();
	// 	// mTrackTuple.localTime = step->GetPreStepPoint()->GetLocalTime();
	// }
}

// if (inciVolumeID == 2 && finalVolumeID == 1) {
//     // 검출기에서 나간 입자
// }

// if (inciVolumeID == 3 && finalVolumeID == 2) {
//     // 콜리메이터를 통과해서 검출기에 도달한 입자
// }



void TAnalysisManager::setFileName(const G4String& name) {
	mFileName = name;
}