
#include <despot/model_primitives/Bp/actionManager.h>
#include <despot/util/mongoDB_Bridge.h>
#include <nlohmann/json.hpp> 

// for convenience
using json = nlohmann::json;
//#include "actionManager.h"
#include <vector>
#include <utility>
#include <string>
namespace despot { 
    void ActionDescription::SetActionParametersByState(BpState *state, std::vector<std::string> indexes){}
    std::vector<ActionDescription*> ActionManager::actions;


void NavigateActionDescription::SetActionParametersByState(BpState *state, std::vector<std::string> indexes)
{
    strLink_oDirection = indexes[0];
    oDirection = (state->tDirectionObjectsForActions[indexes[0]]);
}
std::string NavigateActionDescription::GetActionParametersJson_ForActionExecution()
{  
    json j;
    j["ParameterLinks"]["oDirection"] = strLink_oDirection;
    j["ParameterValues"]["oDirection"] = oDirection;

    std::string str(j.dump().c_str());
    return str;
}
std::string NavigateActionDescription::GetActionParametersJson_ForActionRegistration()
{
    json j;
    j["oDirection"] = oDirection;

    std::string str(j.dump().c_str());
    return str;
}

void ActionManager::Init(BpState* state)
{
	
	int id = 0;
    NavigateActionDescription* navigateActions = new NavigateActionDescription[4];
    std::vector<std::string> indexes;
    int i = 0;
    map<std::string, tDirection>::iterator it1;
    for (it1 = state->tDirectionObjectsForActions.begin(); it1 != state->tDirectionObjectsForActions.end(); it1++)
    {
        indexes.push_back(it1->first);
        NavigateActionDescription &oNavigateAction = navigateActions[i];
        oNavigateAction.SetActionParametersByState(state, indexes);
        oNavigateAction.actionId = id++;
        oNavigateAction.actionType = navigateAction;
        ActionManager::actions.push_back(&oNavigateAction);
        i++;
        indexes.pop_back();


        for(int j=0;j< ActionManager::actions.size();j++)
        {
            std::string actDesc = Prints::PrintActionDescription(ActionManager::actions[j]);
            MongoDB_Bridge::RegisterAction(ActionManager::actions[j]->actionId, enum_map_Bp::vecActionTypeEnumToString[ActionManager::actions[j]->actionType], ActionManager::actions[j]->GetActionParametersJson_ForActionRegistration(), actDesc);
        }
    }
}

 
    std::string Prints::PrinttDirection(tDirection enumT)
    {
        switch (enumT)
        {
            case Up:
                return "Up";
            case Down:
                return "Down";
            case Left:
                return "Left";
            case Right:
                return "Right";
        }
    }

    std::string Prints::PrinttCell(tCell enumT)
    {
        switch (enumT)
        {
            case L00:
                return "L00";
            case L10:
                return "L10";
            case L01:
                return "L01";
            case L11:
                return "L11";
            case OUT:
                return "OUT";
        }
    }

    std::string Prints::PrintActionDescription(ActionDescription* act)
    {
        stringstream ss;
        ss << "ID:" << act->actionId;
        ss << "," << PrintActionType(act->actionType);
        if(act->actionType == navigateAction)
        {
            NavigateActionDescription *navigateA = static_cast<NavigateActionDescription *>(act);
            ss << "," << "oDirection:" << Prints::PrinttDirection((tDirection)navigateA->oDirection);;
        }

        return ss.str();
    }


std::string Prints::PrintObs(int action, int obs)
{
	BpResponseModuleAndTempEnums eObs = (BpResponseModuleAndTempEnums)obs;
	return enum_map_Bp::vecResponseEnumToString[eObs]; 
}
    std::string Prints::PrintState(BpState state)
    {
        stringstream ss;
        ss << "STATE: ";
        ss << "|agentOneLoc:";
        ss <<  Prints::PrinttCell(state.agentOneLoc);
        ss << "|agentTwoLoc:";
        ss <<  Prints::PrinttCell(state.agentTwoLoc);
        ss << "|bOneLoc:";
        ss <<  Prints::PrinttCell(state.bOneLoc);
        ss << "|bTwoLoc:";
        ss <<  Prints::PrinttCell(state.bTwoLoc);
        ss << "|isAgentOneTurn:";
        ss <<  state.isAgentOneTurn;
        ss << "|ParamUp:";
        ss <<  Prints::PrinttDirection(state.ParamUp);
        ss << "|ParamDown:";
        ss <<  Prints::PrinttDirection(state.ParamDown);
        ss << "|ParamLeft:";
        ss <<  Prints::PrinttDirection(state.ParamLeft);
        ss << "|ParamRight:";
        ss <<  Prints::PrinttDirection(state.ParamRight);
        return ss.str();
    }



    std::string Prints::PrintActionType(ActionType actType)
    {
        switch (actType)
        {
        case navigateAction:
            return "navigateAction";
        }
    }


}