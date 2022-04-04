
#include <despot/model_primitives/Bp_with_int_model/actionManager.h>
#include <despot/util/mongoDB_Bridge.h>
#include <nlohmann/json.hpp> 

// for convenience
using json = nlohmann::json;
//#include "actionManager.h"
#include <vector>
#include <utility>
#include <string>
namespace despot { 
    void ActionDescription::SetActionParametersByState(Bp_with_int_modelState *state, std::vector<std::string> indexes){}
    std::vector<ActionDescription*> ActionManager::actions;


void PushActionDescription::SetActionParametersByState(Bp_with_int_modelState *state, std::vector<std::string> indexes)
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
}void NavigateActionDescription::SetActionParametersByState(Bp_with_int_modelState *state, std::vector<std::string> indexes)
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

void ActionManager::Init(Bp_with_int_modelState* state)
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
        MongoDB_Bridge::RegisterAction(ActionManager::actions[j]->actionId, enum_map_Bp_with_int_model::vecActionTypeEnumToString[ActionManager::actions[j]->actionType], ActionManager::actions[j]->GetActionParametersJson_ForActionRegistration(), actDesc);
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
	Bp_with_int_modelResponseModuleAndTempEnums eObs = (Bp_with_int_modelResponseModuleAndTempEnums)obs;
	return enum_map_Bp_with_int_model::vecResponseEnumToString[eObs]; 
}
    std::string Prints::PrintState(Bp_with_int_modelState state)
    {
        stringstream ss;
        ss << "STATE: ";
        ss << "|state.bTwoLocGoal.x:";
        ss << state.bTwoLocGoal.x;
        ss << "|state.bTwoLocGoal.y:";
        ss << state.bTwoLocGoal.y;
        ss << "|state.bOneLocGoal.x:";
        ss << state.bOneLocGoal.x;
        ss << "|state.bOneLocGoal.y:";
        ss << state.bOneLocGoal.y;
        ss << "|state.agentOneLoc.x:";
        ss << state.agentOneLoc.x;
        ss << "|state.agentOneLoc.y:";
        ss << state.agentOneLoc.y;
        ss << "|state.agentTwoLoc.x:";
        ss << state.agentTwoLoc.x;
        ss << "|state.agentTwoLoc.y:";
        ss << state.agentTwoLoc.y;
        ss << "|state.bOneLoc.x:";
        ss << state.bOneLoc.x;
        ss << "|state.bOneLoc.y:";
        ss << state.bOneLoc.y;
        ss << "|state.bTwoLoc.x:";
        ss << state.bTwoLoc.x;
        ss << "|state.bTwoLoc.y:";
        ss << state.bTwoLoc.y;
        ss << "|state.isAgentOneTurn:";
        ss << state.isAgentOneTurn;
        ss << "|state.JointPushDirection:";
        ss << state.JointPushDirection;
        ss << "|state.MaxGridx:";
        ss << state.MaxGridx;
        ss << "|state.MaxGridy:";
        ss << state.MaxGridy;
        return ss.str();
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

std::string Prints::GetStateJson(State& _state)
    {
        const Bp_with_int_modelState& state = static_cast<const Bp_with_int_modelState&>(_state);
        json j;
        j["bTwoLocGoal"]["x"] = state.bTwoLocGoal.x;
        j["bTwoLocGoal"]["y"] = state.bTwoLocGoal.y;
        j["bOneLocGoal"]["x"] = state.bOneLocGoal.x;
        j["bOneLocGoal"]["y"] = state.bOneLocGoal.y;
        j["agentOneLoc"]["x"] = state.agentOneLoc.x;
        j["agentOneLoc"]["y"] = state.agentOneLoc.y;
        j["agentTwoLoc"]["x"] = state.agentTwoLoc.x;
        j["agentTwoLoc"]["y"] = state.agentTwoLoc.y;
        j["bOneLoc"]["x"] = state.bOneLoc.x;
        j["bOneLoc"]["y"] = state.bOneLoc.y;
        j["bTwoLoc"]["x"] = state.bTwoLoc.x;
        j["bTwoLoc"]["y"] = state.bTwoLoc.y;
        j["isAgentOneTurn"] = state.isAgentOneTurn;
        j["ParamUp"] = state.ParamUp;
        j["ParamDown"] = state.ParamDown;
        j["ParamLeft"] = state.ParamLeft;
        j["ParamRight"] = state.ParamRight;
        j["ParamSingleAgentPush"] = state.ParamSingleAgentPush;
        j["ParamJointPush"] = state.ParamJointPush;
        j["JointPushDirection"] = state.JointPushDirection;
        j["MaxGridx"] = state.MaxGridx;
        j["MaxGridy"] = state.MaxGridy;

    std::string str(j.dump().c_str());
    return str;
     
    }

    void Prints::GetStateFromJson(Bp_with_int_modelState& state, std::string jsonStr, int stateIndex)
    {
        
        json j = json::parse(jsonStr);
        j = j["BeliefeState"];

        state.bTwoLocGoal.x = j[stateIndex]["bTwoLocGoal"]["x"];
        state.bTwoLocGoal.y = j[stateIndex]["bTwoLocGoal"]["y"];
        state.bOneLocGoal.x = j[stateIndex]["bOneLocGoal"]["x"];
        state.bOneLocGoal.y = j[stateIndex]["bOneLocGoal"]["y"];
        state.agentOneLoc.x = j[stateIndex]["agentOneLoc"]["x"];
        state.agentOneLoc.y = j[stateIndex]["agentOneLoc"]["y"];
        state.agentTwoLoc.x = j[stateIndex]["agentTwoLoc"]["x"];
        state.agentTwoLoc.y = j[stateIndex]["agentTwoLoc"]["y"];
        state.bOneLoc.x = j[stateIndex]["bOneLoc"]["x"];
        state.bOneLoc.y = j[stateIndex]["bOneLoc"]["y"];
        state.bTwoLoc.x = j[stateIndex]["bTwoLoc"]["x"];
        state.bTwoLoc.y = j[stateIndex]["bTwoLoc"]["y"];
        state.isAgentOneTurn = j[stateIndex]["isAgentOneTurn"];
        state.ParamUp = j[stateIndex]["ParamUp"];
        state.ParamDown = j[stateIndex]["ParamDown"];
        state.ParamLeft = j[stateIndex]["ParamLeft"];
        state.ParamRight = j[stateIndex]["ParamRight"];
        state.ParamSingleAgentPush = j[stateIndex]["ParamSingleAgentPush"];
        state.ParamJointPush = j[stateIndex]["ParamJointPush"];
        state.JointPushDirection = j[stateIndex]["JointPushDirection"];
        state.MaxGridx = j[stateIndex]["MaxGridx"];
        state.MaxGridy = j[stateIndex]["MaxGridy"];

    }



void Prints::SaveBeliefParticles(vector<State*> particles)
{
    json j;
    j["ActionSequnceId"] =  MongoDB_Bridge::currentActionSequenceId;

    for (int i = 0; i < particles.size(); i++)
    {
        j["BeliefeState"][i] = json::parse(Prints::GetStateJson(*particles[i])); 
    }
    
    std::string str(j.dump().c_str());

    j["ActionSequnceId"] = -1;

    std::string currentBeliefStr(j.dump().c_str());
    MongoDB_Bridge::SaveBeliefState(str, currentBeliefStr);
}
}