#ifndef STATE_H
#define STATE_H
#include <vector>
#include <despot/core/pomdp.h> 
#include "state_var_types.h"
namespace despot
{


class BpState : public State {
public:
    std::vector<tPushType> tPushTypeObjects;
    std::vector<tDirection> tDirectionObjects;
    std::vector<tCell> tCellObjects;
    std::map<std::string, tDirection> tDirectionObjectsForActions;
    std::map<std::string, tPushType> tPushTypeObjectsForActions;
    tCell agentOneLoc;
    tCell agentTwoLoc;
    tCell bOneLoc;
    tCell bTwoLoc;
    bool isAgentOneTurn;
    tDirection ParamUp;
    tDirection ParamDown;
    tDirection ParamLeft;
    tDirection ParamRight;
    tPushType ParamSingleAgentPush;
    tPushType ParamJointPush;
    tDirection JointPushDirection;
    std::map<std::string, anyValue*> anyValueUpdateDic;


	public:
		static void SetAnyValueLinks(BpState *state);
		
};
}
#endif //STATE_H