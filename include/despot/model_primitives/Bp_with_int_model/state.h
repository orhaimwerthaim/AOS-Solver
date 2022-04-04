#ifndef STATE_H
#define STATE_H
#include <vector>
#include <despot/core/pomdp.h> 
#include "state_var_types.h"
namespace despot
{


class Bp_with_int_modelState : public State {
public:
    bool OneTimeRewardUsed[1]={true};
    std::vector<tPushType> tPushTypeObjects;
    std::vector<tDirection> tDirectionObjects;
    std::vector<tLocation*> tLocationObjects;
    std::map<std::string, tDirection> tDirectionObjectsForActions;
    std::map<std::string, tPushType> tPushTypeObjectsForActions;
    tLocation bTwoLocGoal;
    tLocation bOneLocGoal;
    tLocation agentOneLoc;
    tLocation agentTwoLoc;
    tLocation bOneLoc;
    tLocation bTwoLoc;
    bool isAgentOneTurn;
    tDirection ParamUp;
    tDirection ParamDown;
    tDirection ParamLeft;
    tDirection ParamRight;
    tPushType ParamSingleAgentPush;
    tPushType ParamJointPush;
    tDirection JointPushDirection;
    int MaxGridx;
    int MaxGridy;
    std::map<std::string, anyValue*> anyValueUpdateDic;


	public:
		static void SetAnyValueLinks(Bp_with_int_modelState *state);
		
};
}
#endif //STATE_H