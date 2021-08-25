
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


void PushActionDescription::SetActionParametersByState(BpState *state, std::vector<std::string> indexes)
{
    strLink_oDirection = indexes[0];
    oDirection = (state->tDirectionObjectsForActions[indexes[0]]);
    strLink_oIsJointPush = indexes[1];
    oIsJointPush = (state->tPushTypeObjectsForActions[indexes[1]]);
}
std::string PushActionDescription::GetActionParametersJson_ForActionExecution()
{  
    json j;
    j["ParameterLinks"]["oDirection"] = strLink_oDirection;
    j["ParameterValues"]["oDirection"] = oDirection;
    j["ParameterLinks"]["oIsJointPush"] = strLink_oIsJointPush;
    j["ParameterValues"]["oIsJointPush"] = oIsJointPush;

    std::string str(j.dump().c_str());
    return str;
}
std::string PushActionDescription::GetActionParametersJson_ForActionRegistration()
{
    json j;
    j["oDirection"] = oDirection;
    j["oIsJointPush"] = oIsJointPush;

    std::string str(j.dump().c_str());
    return str;
}void NavigateActionDescription::SetActionParametersByState(BpState *state, std::vector<std::string> indexes)
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
    PushActionDescription* pushActions = new PushActionDescription[8];
    std::vector<std::string> pushIndexes;
    int pushActCounter = 0;
    map<std::string, tDirection>::iterator pushIt1;
    for (pushIt1 = state->tDirectionObjectsForActions.begin(); pushIt1 != state->tDirectionObjectsForActions.end(); pushIt1++)
    {
        pushIndexes.push_back(pushIt1->first);
        map<std::string, tPushType>::iterator pushIt2;
        for (pushIt2 = state->tPushTypeObjectsForActions.begin(); pushIt2 != state->tPushTypeObjectsForActions.end(); pushIt2++)
        {
            pushIndexes.push_back(pushIt2->first);
            PushActionDescription &oPushAction = pushActions[pushActCounter];
            oPushAction.SetActionParametersByState(state, pushIndexes);
            oPushAction.actionId = id++;
            oPushAction.actionType = pushAction;
            ActionManager::actions.push_back(&oPushAction);
            pushActCounter++;
            pushIndexes.pop_back();
        }
            pushIndexes.pop_back();
    }
    NavigateActionDescription* navigateActions = new NavigateActionDescription[4];
    std::vector<std::string> navigateIndexes;
    int navigateActCounter = 0;
    map<std::string, tDirection>::iterator navigateIt1;
    for (navigateIt1 = state->tDirectionObjectsForActions.begin(); navigateIt1 != state->tDirectionObjectsForActions.end(); navigateIt1++)
    {
        navigateIndexes.push_back(navigateIt1->first);
        NavigateActionDescription &oNavigateAction = navigateActions[navigateActCounter];
        oNavigateAction.SetActionParametersByState(state, navigateIndexes);
        oNavigateAction.actionId = id++;
        oNavigateAction.actionType = navigateAction;
        ActionManager::actions.push_back(&oNavigateAction);
        navigateActCounter++;
        navigateIndexes.pop_back();
    }


    for(int j=0;j< ActionManager::actions.size();j++)
    {
        std::string actDesc = Prints::PrintActionDescription(ActionManager::actions[j]);
        MongoDB_Bridge::RegisterAction(ActionManager::actions[j]->actionId, enum_map_Bp::vecActionTypeEnumToString[ActionManager::actions[j]->actionType], ActionManager::actions[j]->GetActionParametersJson_ForActionRegistration(), actDesc);
    }
}


 
    std::string Prints::PrinttPushType(tPushType enumT)
    {
        switch (enumT)
        {
            case SingleAgentPush:
                return "SingleAgentPush";
            case JointPush:
                return "JointPush";
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
            case None:
                return "None";
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
        if(act->actionType == pushAction)
        {
            PushActionDescription *pushA = static_cast<PushActionDescription *>(act);
            ss << "," << "oDirection:" << Prints::PrinttDirection((tDirection)pushA->oDirection);;
            ss << "," << "oIsJointPush:" << Prints::PrinttPushType((tPushType)pushA->oIsJointPush);;
        }

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
        ss << "|ParamSingleAgentPush:";
        ss <<  Prints::PrinttPushType(state.ParamSingleAgentPush);
        ss << "|ParamJointPush:";
        ss <<  Prints::PrinttPushType(state.ParamJointPush);
        ss << "|JointPushDirection:";
        ss <<  Prints::PrinttDirection(state.JointPushDirection);
        return ss.str();
    }




    std::string Prints::GetStateJson(State& _state)
    {
        const BpState& state = static_cast<const BpState&>(_state);
        json j;
    j["agentOneLoc"] = Prints::PrinttCell(state.agentOneLoc);
    j["agentTwoLoc"] = Prints::PrinttCell(state.agentTwoLoc);
    j["isAgentOneTurn"] = Prints::PrinttCell(state.bOneLoc);
    //j["stateTest"] = state;



     

    std::string str(j.dump().c_str());
    return str;
     
    }


    std::string Prints::PrintActionType(ActionType actType)
    {
        switch (actType)
        {
        case pushAction:
            return "pushAction";
        case navigateAction:
            return "navigateAction";
        }
    }


std::string Prints::GetJsonForBelief(vector<State*> particles)
{
    json j;
    j["ActionSequnceId"] =  MongoDB_Bridge::currentActionSequenceId;

    for (int i = 0; i < particles.size(); i++)
    {
        j["BeliefeState"][i] = json::parse(Prints::GetStateJson(*particles[0])); 
    }
    
    std::string str(j.dump().c_str());
    MongoDB_Bridge::SaveBeliefState(str);
    return str;
}
}