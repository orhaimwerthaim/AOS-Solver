#ifndef STATE_H
#define STATE_H
#include <vector>
#include <despot/core/pomdp.h> 
#include "state_var_types.h"
namespace despot
{


class IcapsState : public State {
public:
    bool OneTimeRewardUsed[1]={true};
    std::vector<tDiscreteLocation> tDiscreteLocationObjects;
    std::map<std::string, tLocation> tLocationObjectsForActions;
    tDiscreteLocation cupDiscreteGeneralLocation;
    anyValue cupAccurateLocation;
    bool handEmpty;
    tDiscreteLocation robotGenerallocation;
    tLocation locationOutside_lab211;
    tLocation locationAuditorium;
    tLocation locationNear_elevator1;
    tLocation locationCorridor;
    std::map<std::string, anyValue*> anyValueUpdateDic;


	public:
		static void SetAnyValueLinks(IcapsState *state);
		
};
}
#endif //STATE_H