#ifndef STATE_H
#define STATE_H
#include <vector>
#include <despot/core/pomdp.h> 
#include "state_var_types.h"
namespace despot
{
class IcapsState : public State {
public:
	std::vector<tDiscreteLocation> tDiscreteLocationObjects;
	std::vector<tLocation> tLocationObjects;

	tDiscreteLocation cupDiscreteGeneralLocation;
	tDiscreteLocation robotGenerallocation;

	tLocation locationOutside_lab211;
	tLocation locationAuditorium;
	tLocation locationNear_elevator1;
	tLocation locationCorridor;

	anyValue cupAccurateLocation;
	std::map<std::string, anyValue> anyValueUpdateDic;
	bool handEmpty;
};
}
#endif //STATE_H