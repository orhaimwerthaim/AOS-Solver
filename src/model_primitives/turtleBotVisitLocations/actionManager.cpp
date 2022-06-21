
#include <despot/model_primitives/turtleBotVisitLocations/actionManager.h>
#include <despot/util/mongoDB_Bridge.h>
#include <nlohmann/json.hpp> 

// for convenience
using json = nlohmann::json;
//#include "actionManager.h"
#include <vector>
#include <utility>
#include <string>
namespace despot { 
    void ActionDescription::SetActionParametersByState(TurtleBotVisitLocationsState *state, std::vector<std::string> indexes){}
    std::vector<ActionDescription*> ActionManager::actions;


void NavigateActionDescription::SetActionParametersByState(TurtleBotVisitLocationsState *state, std::vector<std::string> indexes)
{
    strLink_oDesiredLocation = indexes[0];
    oDesiredLocation = (state->tLocationObjectsForActions[indexes[0]]);
}
std::string NavigateActionDescription::GetActionParametersJson_ForActionExecution()
{  
    json j;
    j["ParameterLinks"]["oDesiredLocation"] = strLink_oDesiredLocation;
    j["ParameterValues"]["oDesiredLocation"]["x"] = oDesiredLocation.x;
    j["ParameterValues"]["oDesiredLocation"]["y"] = oDesiredLocation.y;
    j["ParameterValues"]["oDesiredLocation"]["z"] = oDesiredLocation.z;
    j["ParameterValues"]["oDesiredLocation"]["desc"] = oDesiredLocation.desc;

    std::string str(j.dump().c_str());
    return str;
}
std::string NavigateActionDescription::GetActionParametersJson_ForActionRegistration()
{
    json j;
    j["oDesiredLocation->x"] = oDesiredLocation.x;
    j["oDesiredLocation->y"] = oDesiredLocation.y;
    j["oDesiredLocation->z"] = oDesiredLocation.z;
    j["oDesiredLocation->desc"] = oDesiredLocation.desc;

    std::string str(j.dump().c_str());
    return str;
}

void ActionManager::Init(TurtleBotVisitLocationsState* state)
{
	
	int id = 0;
    NavigateActionDescription* navigateActions = new NavigateActionDescription[7];
    std::vector<std::string> navigateIndexes;
    int navigateActCounter = 0;
    map<std::string, tLocation>::iterator navigateIt1;
    for (navigateIt1 = state->tLocationObjectsForActions.begin(); navigateIt1 != state->tLocationObjectsForActions.end(); navigateIt1++)
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
        MongoDB_Bridge::RegisterAction(ActionManager::actions[j]->actionId, enum_map_turtleBotVisitLocations::vecActionTypeEnumToString[ActionManager::actions[j]->actionType], ActionManager::actions[j]->GetActionParametersJson_ForActionRegistration(), actDesc);
    }
}


 
    std::string Prints::PrinttDiscreteLocation(tDiscreteLocation enumT)
    {
        switch (enumT)
        {
            case eL1:
                return "eL1";
            case eL2:
                return "eL2";
            case eL3:
                return "eL3";
            case eL4:
                return "eL4";
            case eL5:
                return "eL5";
            case eL6:
                return "eL6";
            case eL7:
                return "eL7";
            case eL8:
                return "eL8";
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
            ss << "," << "x:" << navigateA->oDesiredLocation.x;
            ss << "," << "y:" << navigateA->oDesiredLocation.y;
            ss << "," << "z:" << navigateA->oDesiredLocation.z;
            ss << "," << "desc:" << Prints::PrinttDiscreteLocation((tDiscreteLocation)navigateA->oDesiredLocation.desc);;
        }

        return ss.str();
    }


std::string Prints::PrintObs(int action, int obs)
{
	TurtleBotVisitLocationsResponseModuleAndTempEnums eObs = (TurtleBotVisitLocationsResponseModuleAndTempEnums)obs;
	return enum_map_turtleBotVisitLocations::vecResponseEnumToString[eObs]; 
}
    std::string Prints::PrintState(TurtleBotVisitLocationsState state)
    {
        stringstream ss;
        ss << "STATE: ";
        ss << "|state.v1.visited:";
        ss << state.v1.visited;
        ss << "|state.v1.desc:";
        ss << state.v1.desc;
        ss << "|state.v2.visited:";
        ss << state.v2.visited;
        ss << "|state.v2.desc:";
        ss << state.v2.desc;
        ss << "|state.v3.visited:";
        ss << state.v3.visited;
        ss << "|state.v3.desc:";
        ss << state.v3.desc;
        ss << "|state.v4.visited:";
        ss << state.v4.visited;
        ss << "|state.v4.desc:";
        ss << state.v4.desc;
        ss << "|state.v5.visited:";
        ss << state.v5.visited;
        ss << "|state.v5.desc:";
        ss << state.v5.desc;
        ss << "|state.v6.visited:";
        ss << state.v6.visited;
        ss << "|state.v6.desc:";
        ss << state.v6.desc;
        ss << "|state.v7.visited:";
        ss << state.v7.visited;
        ss << "|state.v7.desc:";
        ss << state.v7.desc;
        ss << "|state.l8.x:";
        ss << state.l8.x;
        ss << "|state.l8.y:";
        ss << state.l8.y;
        ss << "|state.l8.z:";
        ss << state.l8.z;
        ss << "|state.l8.desc:";
        ss << state.l8.desc;
        ss << "|state.robotL.x:";
        ss << state.robotL.x;
        ss << "|state.robotL.y:";
        ss << state.robotL.y;
        ss << "|state.robotL.z:";
        ss << state.robotL.z;
        ss << "|state.robotL.desc:";
        ss << state.robotL.desc;
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

std::string Prints::GetStateJson(State& _state)
    {
        const TurtleBotVisitLocationsState& state = static_cast<const TurtleBotVisitLocationsState&>(_state);
        json j;
        j["v1"]["visited"] = state.v1.visited;
        j["v1"]["desc"] = state.v1.desc;
        j["v2"]["visited"] = state.v2.visited;
        j["v2"]["desc"] = state.v2.desc;
        j["v3"]["visited"] = state.v3.visited;
        j["v3"]["desc"] = state.v3.desc;
        j["v4"]["visited"] = state.v4.visited;
        j["v4"]["desc"] = state.v4.desc;
        j["v5"]["visited"] = state.v5.visited;
        j["v5"]["desc"] = state.v5.desc;
        j["v6"]["visited"] = state.v6.visited;
        j["v6"]["desc"] = state.v6.desc;
        j["v7"]["visited"] = state.v7.visited;
        j["v7"]["desc"] = state.v7.desc;
        j["l1"]["x"] = state.l1.x;
        j["l1"]["y"] = state.l1.y;
        j["l1"]["z"] = state.l1.z;
        j["l1"]["desc"] = state.l1.desc;
        j["l2"]["x"] = state.l2.x;
        j["l2"]["y"] = state.l2.y;
        j["l2"]["z"] = state.l2.z;
        j["l2"]["desc"] = state.l2.desc;
        j["l3"]["x"] = state.l3.x;
        j["l3"]["y"] = state.l3.y;
        j["l3"]["z"] = state.l3.z;
        j["l3"]["desc"] = state.l3.desc;
        j["l4"]["x"] = state.l4.x;
        j["l4"]["y"] = state.l4.y;
        j["l4"]["z"] = state.l4.z;
        j["l4"]["desc"] = state.l4.desc;
        j["l5"]["x"] = state.l5.x;
        j["l5"]["y"] = state.l5.y;
        j["l5"]["z"] = state.l5.z;
        j["l5"]["desc"] = state.l5.desc;
        j["l6"]["x"] = state.l6.x;
        j["l6"]["y"] = state.l6.y;
        j["l6"]["z"] = state.l6.z;
        j["l6"]["desc"] = state.l6.desc;
        j["l7"]["x"] = state.l7.x;
        j["l7"]["y"] = state.l7.y;
        j["l7"]["z"] = state.l7.z;
        j["l7"]["desc"] = state.l7.desc;
        j["l8"]["x"] = state.l8.x;
        j["l8"]["y"] = state.l8.y;
        j["l8"]["z"] = state.l8.z;
        j["l8"]["desc"] = state.l8.desc;
        j["robotL"]["x"] = state.robotL.x;
        j["robotL"]["y"] = state.robotL.y;
        j["robotL"]["z"] = state.robotL.z;
        j["robotL"]["desc"] = state.robotL.desc;

    std::string str(j.dump().c_str());
    return str;
     
    }

    void Prints::GetStateFromJson(TurtleBotVisitLocationsState& state, std::string jsonStr, int stateIndex)
    {
        
        json j = json::parse(jsonStr);
        j = j["BeliefeState"];

        state.v1.visited = j[stateIndex]["v1"]["visited"];
        state.v1.desc = j[stateIndex]["v1"]["desc"];
        state.v2.visited = j[stateIndex]["v2"]["visited"];
        state.v2.desc = j[stateIndex]["v2"]["desc"];
        state.v3.visited = j[stateIndex]["v3"]["visited"];
        state.v3.desc = j[stateIndex]["v3"]["desc"];
        state.v4.visited = j[stateIndex]["v4"]["visited"];
        state.v4.desc = j[stateIndex]["v4"]["desc"];
        state.v5.visited = j[stateIndex]["v5"]["visited"];
        state.v5.desc = j[stateIndex]["v5"]["desc"];
        state.v6.visited = j[stateIndex]["v6"]["visited"];
        state.v6.desc = j[stateIndex]["v6"]["desc"];
        state.v7.visited = j[stateIndex]["v7"]["visited"];
        state.v7.desc = j[stateIndex]["v7"]["desc"];
        state.l1.x = j[stateIndex]["l1"]["x"];
        state.l1.y = j[stateIndex]["l1"]["y"];
        state.l1.z = j[stateIndex]["l1"]["z"];
        state.l1.desc = j[stateIndex]["l1"]["desc"];
        state.l2.x = j[stateIndex]["l2"]["x"];
        state.l2.y = j[stateIndex]["l2"]["y"];
        state.l2.z = j[stateIndex]["l2"]["z"];
        state.l2.desc = j[stateIndex]["l2"]["desc"];
        state.l3.x = j[stateIndex]["l3"]["x"];
        state.l3.y = j[stateIndex]["l3"]["y"];
        state.l3.z = j[stateIndex]["l3"]["z"];
        state.l3.desc = j[stateIndex]["l3"]["desc"];
        state.l4.x = j[stateIndex]["l4"]["x"];
        state.l4.y = j[stateIndex]["l4"]["y"];
        state.l4.z = j[stateIndex]["l4"]["z"];
        state.l4.desc = j[stateIndex]["l4"]["desc"];
        state.l5.x = j[stateIndex]["l5"]["x"];
        state.l5.y = j[stateIndex]["l5"]["y"];
        state.l5.z = j[stateIndex]["l5"]["z"];
        state.l5.desc = j[stateIndex]["l5"]["desc"];
        state.l6.x = j[stateIndex]["l6"]["x"];
        state.l6.y = j[stateIndex]["l6"]["y"];
        state.l6.z = j[stateIndex]["l6"]["z"];
        state.l6.desc = j[stateIndex]["l6"]["desc"];
        state.l7.x = j[stateIndex]["l7"]["x"];
        state.l7.y = j[stateIndex]["l7"]["y"];
        state.l7.z = j[stateIndex]["l7"]["z"];
        state.l7.desc = j[stateIndex]["l7"]["desc"];
        state.l8.x = j[stateIndex]["l8"]["x"];
        state.l8.y = j[stateIndex]["l8"]["y"];
        state.l8.z = j[stateIndex]["l8"]["z"];
        state.l8.desc = j[stateIndex]["l8"]["desc"];
        state.robotL.x = j[stateIndex]["robotL"]["x"];
        state.robotL.y = j[stateIndex]["robotL"]["y"];
        state.robotL.z = j[stateIndex]["robotL"]["z"];
        state.robotL.desc = j[stateIndex]["robotL"]["desc"];

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