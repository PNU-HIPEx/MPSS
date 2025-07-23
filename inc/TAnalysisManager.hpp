#ifndef __TANALYSISMANAGER__
#define __TANALYSISMANAGER__

#include "G4String.hh"
#include "TROOT.h"
#include "TConfig.hpp"

class TFile;
class TTree;
class ProgressBar;
class G4LogicalVolume;
class G4Run;
class G4Event;
class G4Track;
class G4Step;

struct trackTuple {
	Int_t eventID, trackID, parentID, particleID;
	Double_t initX, initY, initZ, initPX, initPY, initPZ, initKineticEnergy;
	Int_t initVolumeID;
	Double_t finalX, finalY, finalZ, finalPX, finalPY, finalPZ, finalKineticEnergy;
	Int_t finalVolumeID;
	Double_t incidentPosition[3], incidentMomentum[3], incidentKineticEnergy;
	Double_t globalTime, localTime;

	void init() {
		eventID = 0;
		trackID = 0;
		parentID = 0;
		particleID = 0;
		initX = 0;
		initY = 0;
		initZ = 0;
		initPX = 0;
		initPY = 0;
		initPZ = 0;
		initKineticEnergy = 0;
		initVolumeID = 0;
		finalX = 0;
		finalY = 0;
		finalZ = 0;
		finalPX = 0;
		finalPY = 0;
		finalPZ = 0;
		finalKineticEnergy = 0;
		finalVolumeID = 0;
	}
};

struct PARTICLE {
	enum {
		unknown = 0,
		alpha = 1,
		electron = 2,
		gammaRay = 3,
		proton = 4,
		neutron = 5
	};
};

class TAnalysisManager {
public:
	TAnalysisManager();
	TAnalysisManager(const KEI::TConfigFile& config);
	~TAnalysisManager();

private:
	static TAnalysisManager* mInstance;

	KEI::TConfigFile mConfig;

	trackTuple mTrackTuple;

	G4String mFileName;
	TFile* mFile = nullptr;
	TTree* mTrackTree = nullptr;
	ProgressBar* mProgressBar = nullptr;
	std::vector<std::string> mUnknownParticleList;

	Int_t mEventID = 0;

	G4LogicalVolume* mWorldLogical = nullptr;
	G4LogicalVolume* mTungstenLogical = nullptr;
	G4LogicalVolume* mGlassLogical = nullptr;
	G4LogicalVolume* mDetectorLogical = nullptr;

	G4bool isInALPIDE = false;
public:
	static TAnalysisManager* Instance();

	void open();
	void close();

	void doBeginOfRun(const G4Run* run);
	void doEndOfRun(const G4Run* run);
	void doBeginOfEvent(const G4Event* event);
	void doEndOfEvent(const G4Event* event);
	void doPreTracking(const G4Track* track);
	void doPostTracking(const G4Track* track);
	void doStepPhase(const G4Step* step);

	void setFileName(const G4String& name);

	Int_t getParticleID(const G4String& particleID);
	Int_t getVolumeID(const G4LogicalVolume* volumeID);
};

#endif