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

TAnalysisManager::~TAnalysisManager() {
	close();
}

TAnalysisManager* TAnalysisManager::Instance() {
	if ( mInstance == 0 ) {
		mInstance = new TAnalysisManager();
	}
	return mInstance;
}

void TAnalysisManager::open(const G4String& name) {
	mFileName = name;
	std::filesystem::path path = (std::string) name;
	static int fileIndex = 0;
	G4String newPath = path.parent_path().string() + "/" + static_cast<std::string>(path.stem()) + "_" + std::to_string(fileIndex) + static_cast<std::string>(path.extension());
	fileIndex++;

	mFile = new TFile(newPath, "RECREATE");
	mTrackTree = new TTree("trackTree", "Track Information");

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
	mTrackTree->Branch("initKineticEnergy", &mTrackTuple.initKineticEnergy);
	mTrackTree->Branch("initVolumeID", &mTrackTuple.initVolumeID);
	mTrackTree->Branch("finalX", &mTrackTuple.finalX);
	mTrackTree->Branch("finalY", &mTrackTuple.finalY);
	mTrackTree->Branch("finalZ", &mTrackTuple.finalZ);
	mTrackTree->Branch("finalPX", &mTrackTuple.finalPX);
	mTrackTree->Branch("finalPY", &mTrackTuple.finalPY);
	mTrackTree->Branch("finalPZ", &mTrackTuple.finalPZ);
	mTrackTree->Branch("finalKineticEnergy", &mTrackTuple.finalKineticEnergy);
	mTrackTree->Branch("finalVolumeID", &mTrackTuple.finalVolumeID);

	mIncidentTree = new TTree("incidentTree", "Incident Information");
	mIncidentTree->Branch("eventID", &mIncidentTuple.eventID);
	mIncidentTree->Branch("trackID", &mIncidentTuple.trackID);
	mIncidentTree->Branch("depositEnergyMetal", &mIncidentTuple.depositEnergy[0]);
	mIncidentTree->Branch("depositEnergyEpitaxial", &mIncidentTuple.depositEnergy[1]);
	mIncidentTree->Branch("depositEnergySubstrate", &mIncidentTuple.depositEnergy[2]);
	mIncidentTree->Branch("x", &mIncidentTuple.position[0]);
	mIncidentTree->Branch("y", &mIncidentTuple.position[1]);
	mIncidentTree->Branch("z", &mIncidentTuple.position[2]);
	mIncidentTree->Branch("px", &mIncidentTuple.momentum[0]);
	mIncidentTree->Branch("py", &mIncidentTuple.momentum[1]);
	mIncidentTree->Branch("pz", &mIncidentTuple.momentum[2]);
	mIncidentTree->Branch("kineticEnergy", &mIncidentTuple.kineticEnergy);
	mIncidentTree->Branch("globalTime", &mIncidentTuple.globalTime);
	mIncidentTree->Branch("localTime", &mIncidentTuple.localTime);
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

Int_t TAnalysisManager::getVolumeID(const G4LogicalVolume* volume) {
	if ( volume == mWorldLogical ) {
		return 1;
	} else if ( volume == mTungstenLogical ) {
		return 2;
	} else if ( volume == mGlassLogical ) {
		return 3;
	} else if ( volume == mDetectorLogical ) {
		return 4;
	} else {
		return 0;
	}
}

void TAnalysisManager::close() {
	mFile->cd();
	mTrackTree->Write();
	mIncidentTree->Write();
	mFile->Close();
}

void TAnalysisManager::doBeginOfRun(const G4Run* run) {
	mProgressBar = new ProgressBar(static_cast<int>(run->GetNumberOfEventToBeProcessed()));
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
	mProgressBar->printProgress();
	mTrackTuple.eventID = event->GetEventID();
	static bool isFirstEvent = true;
	if ( event->GetEventID() % 2580000 == 0 ) {
		if ( !isFirstEvent ) {
			mFile->cd();
			mTrackTree->Write();
			mIncidentTree->Write();
			mFile->Close();
			open(mFileName);
		} else {
			isFirstEvent = false;
			open(mFileName);
		}
	}
}

void TAnalysisManager::doEndOfEvent(const G4Event* event) { }

void TAnalysisManager::doPreTracking(const G4Track* track) {
	Int_t eventID = mTrackTuple.eventID;
	mTrackTuple.init();
	mIncidentTuple.init();

	mTrackTuple.eventID = eventID;
	mTrackTuple.trackID = track->GetTrackID();
	mTrackTuple.parentID = track->GetParentID();
	mTrackTuple.particleID = getParticleID(track->GetDefinition()->GetParticleName());
	if ( mTrackTuple.particleID == 0 ) {
		if ( std::find(mUnknownParticleList.begin(), mUnknownParticleList.end(), track->GetDefinition()->GetParticleName()) == mUnknownParticleList.end() ) {
			mUnknownParticleList.push_back(track->GetDefinition()->GetParticleName());
		}
	}
	isInDetector = false;
}

void TAnalysisManager::doPostTracking(const G4Track* track) {
	const TDetectorConstruction* detectorConstruction = static_cast<const TDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	if ( mWorldLogical == nullptr ) {
		mWorldLogical = detectorConstruction->getWorldLogical();
	}

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
	if ( isInDetector ) {
		mIncidentTree->Fill();
	}
}

void TAnalysisManager::doStepPhase(const G4Step* step) {
	G4LogicalVolume* currentVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
	G4int volumeID = getVolumeID(currentVolume);

	const TDetectorConstruction* detectorConstruction = static_cast<const TDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	if ( mTungstenLogical == nullptr ) {
		mTungstenLogical = detectorConstruction->getTungstenLogical();
	}
	if ( mGlassLogical == nullptr ) {
		mGlassLogical = detectorConstruction->getGlassLogical();
	}
	if ( mDetectorLogical == nullptr ) {
		mDetectorLogical = detectorConstruction->getDetectorLogical();
	}
	if ( mWorldLogical == nullptr ) {
		mWorldLogical = detectorConstruction->getWorldLogical();
	}
}

void TAnalysisManager::setFileName(const G4String& name) {
	mFileName = name;
}