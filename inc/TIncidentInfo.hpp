#ifndef __TINCIDENTINFO__
#define __TINCIDENTINFO__

#include "G4VUserTrackInformation.hh"

class TIncidentInfo : public G4VUserTrackInformation {
public:
	TIncidentInfo() = default;

	G4bool mIsIncident = false;
	G4ThreeVector mIncidentPosition;
	G4ThreeVector mIncidentMomentum;
	G4double mIncidentKineticEnergy;
};

#endif