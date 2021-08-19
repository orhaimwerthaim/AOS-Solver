#ifndef STATE_H
#define STATE_H
#include <vector>
#include <despot/core/pomdp.h> 
#include "state_var_types.h"
namespace despot
{


class BpState : public State {
public:
    std::vector<tDirection> tDirectionObjects;
    std::vector<tCell> tCellObjects;
    std::map<std::string, tDirection> tDirectionObjectsForActions;
    tCell agentOneLoc;
    tCell agentTwoLoc;
    tCell bOneLoc;
    tCell bTwoLoc;
    bool isAgentOneTurn;
    tDirection ParamUp;
    tDirection ParamDown;
    tDirection ParamLeft;
    tDirection ParamRight;
    std::map<std::string, anyValue*> anyValueUpdateDic;


	public:
		static void SetAnyValueLinks(BpState *state);
		
};
}
#endif //STATE_H