#ifndef __TActionInitialization__
#define __TActionInitialization__

#include "G4VUserActionInitialization.hh"

#include "TConfig.hpp"

class TActionInitialization : public G4VUserActionInitialization {
public:
	TActionInitialization() = default;
	TActionInitialization(const KEI::TConfigFile& config);
	~TActionInitialization() override = default;

private:
	KEI::TConfigFile mConfig;

public:
	void BuildForMaster() const override;
	void Build() const override;
};

#endif