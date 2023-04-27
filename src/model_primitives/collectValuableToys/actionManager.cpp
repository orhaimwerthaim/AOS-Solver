
#include <despot/model_primitives/collectValuableToys/actionManager.h>
#include <despot/util/mongoDB_Bridge.h>
#include <nlohmann/json.hpp> 

// for convenience
using json = nlohmann::json;
//#include "actionManager.h"
#include <vector>
#include <utility>
#include <string>
namespace despot { 
    
    void ActionDescription::SetActionParametersByState(CollectValuableToysState *state, std::vector<std::string> indexes){}
    std::vector<ActionDescription*> ActionManager::actions;


void NavigateActionDescription::SetActionParametersByState(CollectValuableToysState *state, std::vector<std::string> indexes)
{
    strLink_oDestination = indexes[0];
    oDestination = (state->intObjectsForActions[indexes[0]]);
}
std::string NavigateActionDescription::GetActionParametersJson_ForActionExecution()
{  
    json j;
    j["ParameterLinks"]["oDestination"] = strLink_oDestination;
    j["ParameterValues"]["oDestination"] = oDestination;

    std::string str(j.dump().c_str());
    return str;
}
std::string NavigateActionDescription::GetActionParametersJson_ForActionRegistration()
{
    json j;
    j["oDestination"] = oDestination;

    std::string str(j.dump().c_str());
    return str;
}void PickActionDescription::SetActionParametersByState(CollectValuableToysState *state, std::vector<std::string> indexes)
{
    strLink_toyType = indexes[0];
    toyType = (state->stringObjectsForActions[indexes[0]]);
}
std::string PickActionDescription::GetActionParametersJson_ForActionExecution()
{  
    json j;
    j["ParameterLinks"]["toyType"] = strLink_toyType;
    j["ParameterValues"]["toyType"] = toyType;

    std::string str(j.dump().c_str());
    return str;
}
std::string PickActionDescription::GetActionParametersJson_ForActionRegistration()
{
    json j;
    j["toyType"] = toyType;

    std::string str(j.dump().c_str());
    return str;
}

void ActionManager::Init(CollectValuableToysState* state)
{
	
	int id = 0;
    NavigateActionDescription* navigateActions = new NavigateActionDescription[5];
    std::vector<std::string> navigateIndexes;
    int navigateActCounter = 0;
    map<std::string, int>::iterator navigateIt1;
    for (navigateIt1 = state->intObjectsForActions.begin(); navigateIt1 != state->intObjectsForActions.end(); navigateIt1++)
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
    PickActionDescription* pickActions = new PickActionDescription[4];
    std::vector<std::string> pickIndexes;
    int pickActCounter = 0;
    map<std::string, string>::iterator pickIt1;
    for (pickIt1 = state->stringObjectsForActions.begin(); pickIt1 != state->stringObjectsForActions.end(); pickIt1++)
    {
        pickIndexes.push_back(pickIt1->first);
        PickActionDescription &oPickAction = pickActions[pickActCounter];
        oPickAction.SetActionParametersByState(state, pickIndexes);
        oPickAction.actionId = id++;
        oPickAction.actionType = pickAction;
        ActionManager::actions.push_back(&oPickAction);
        pickActCounter++;
        pickIndexes.pop_back();
    }
    ActionDescription *place = new ActionDescription;
    place->actionType = placeAction;
    place->actionId = id++;
    ActionManager::actions.push_back(place);



    for(int j=0;j< ActionManager::actions.size();j++)
    {
        std::string actDesc = Prints::PrintActionDescription(ActionManager::actions[j]);
        MongoDB_Bridge::RegisterAction(ActionManager::actions[j]->actionId, enum_map_collectValuableToys::vecActionTypeEnumToString[ActionManager::actions[j]->actionType], ActionManager::actions[j]->GetActionParametersJson_ForActionRegistration(), actDesc);
    }
    
}
std::string Prints::PrintActionDescription(int actionId)
{
    return Prints::PrintActionDescription(ActionManager::actions[actionId]);
}


 
    std::string Prints::PrintActionDescription(ActionDescription* act)
    {
        stringstream ss;
        ss << "ID:" << act->actionId;
        ss << "," << PrintActionType(act->actionType);
        if(act->actionType == navigateAction)
        {
            NavigateActionDescription *navigateA = static_cast<NavigateActionDescription *>(act);
            ss << "," << "oDestination:" << navigateA->oDestination;
        }

        if(act->actionType == pickAction)
        {
            PickActionDescription *pickA = static_cast<PickActionDescription *>(act);
            ss << "," << "toyType:" << pickA->toyType;
        }

        return ss.str();
    }



    std::string Prints::PrintState(CollectValuableToysState state)
    {
        stringstream ss;
        ss << "STATE: " << endl;
        ss << "|state.robotArm:";
        ss << state.robotArm;
        ss << "|state.toy1.location:";
        ss << state.toy1.location;
        ss << "|state.toy1.type:";
        ss << state.toy1.type;
        ss << "|state.toy1.reward:";
        ss << state.toy1.reward;
        ss << "|state.toy2.location:";
        ss << state.toy2.location;
        ss << "|state.toy2.type:";
        ss << state.toy2.type;
        ss << "|state.toy2.reward:";
        ss << state.toy2.reward;
        ss << "|state.toy3.location:";
        ss << state.toy3.location;
        ss << "|state.toy3.type:";
        ss << state.toy3.type;
        ss << "|state.toy3.reward:";
        ss << state.toy3.reward;
        ss << "|state.toy4.location:";
        ss << state.toy4.location;
        ss << "|state.toy4.type:";
        ss << state.toy4.type;
        ss << "|state.toy4.reward:";
        ss << state.toy4.reward;
        ss << "|state.pickActionsLeft:";
        ss << state.pickActionsLeft;
        ss << "|state.robotLocation:";
        ss << state.robotLocation;
        return ss.str();
    }


 
    std::string Prints::PrintActionType(ActionType actType)
    {
        switch (actType)
        {
        case navigateAction:
            return "navigateAction";
        case pickAction:
            return "pickAction";
        case placeAction:
            return "placeAction";
        }
        return "";
    }

std::string Prints::GetStateJson(State& _state)
    {
        const CollectValuableToysState& state = static_cast<const CollectValuableToysState&>(_state);
        json j;
        j["location0"] = state.location0;
        j["location1"] = state.location1;
        j["location2"] = state.location2;
        j["location3"] = state.location3;
        j["child"] = state.child;
        j["robotArm"] = state.robotArm;
        j["toy_typeGreen"] = state.toy_typeGreen;
        j["toy_typeBlue"] = state.toy_typeBlue;
        j["toy_typeBlack"] = state.toy_typeBlack;
        j["toy_typeRed"] = state.toy_typeRed;
        j["toy1"]["location"] = state.toy1.location;
        j["toy1"]["type"] = state.toy1.type;
        j["toy1"]["reward"] = state.toy1.reward;
        j["toy2"]["location"] = state.toy2.location;
        j["toy2"]["type"] = state.toy2.type;
        j["toy2"]["reward"] = state.toy2.reward;
        j["toy3"]["location"] = state.toy3.location;
        j["toy3"]["type"] = state.toy3.type;
        j["toy3"]["reward"] = state.toy3.reward;
        j["toy4"]["location"] = state.toy4.location;
        j["toy4"]["type"] = state.toy4.type;
        j["toy4"]["reward"] = state.toy4.reward;
        j["pickActionsLeft"] = state.pickActionsLeft;
        j["robotLocation"] = state.robotLocation;

    std::string str(j.dump().c_str());
    return str;
     
    }

    void Prints::GetStateFromJson(CollectValuableToysState& state, std::string jsonStr, int stateIndex)
    {
        
        json j = json::parse(jsonStr);
        j = j["BeliefeState"];

        state.location0 = j[stateIndex]["location0"];
        state.location1 = j[stateIndex]["location1"];
        state.location2 = j[stateIndex]["location2"];
        state.location3 = j[stateIndex]["location3"];
        state.child = j[stateIndex]["child"];
        state.robotArm = j[stateIndex]["robotArm"];
        state.toy_typeGreen = j[stateIndex]["toy_typeGreen"];
        state.toy_typeBlue = j[stateIndex]["toy_typeBlue"];
        state.toy_typeBlack = j[stateIndex]["toy_typeBlack"];
        state.toy_typeRed = j[stateIndex]["toy_typeRed"];
        state.toy1.location = j[stateIndex]["toy1"]["location"];
        state.toy1.type = j[stateIndex]["toy1"]["type"];
        state.toy1.reward = j[stateIndex]["toy1"]["reward"];
        state.toy2.location = j[stateIndex]["toy2"]["location"];
        state.toy2.type = j[stateIndex]["toy2"]["type"];
        state.toy2.reward = j[stateIndex]["toy2"]["reward"];
        state.toy3.location = j[stateIndex]["toy3"]["location"];
        state.toy3.type = j[stateIndex]["toy3"]["type"];
        state.toy3.reward = j[stateIndex]["toy3"]["reward"];
        state.toy4.location = j[stateIndex]["toy4"]["location"];
        state.toy4.type = j[stateIndex]["toy4"]["type"];
        state.toy4.reward = j[stateIndex]["toy4"]["reward"];
        state.pickActionsLeft = j[stateIndex]["pickActionsLeft"];
        state.robotLocation = j[stateIndex]["robotLocation"];

    }



std::string Prints::PrintObs(int obs)
{
	CollectValuableToysResponseModuleAndTempEnums eObs = (CollectValuableToysResponseModuleAndTempEnums)obs;
	return enum_map_collectValuableToys::vecResponseEnumToString[eObs]; 
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



void Prints::SaveSimulatedState(State* state)
{
    json j;
    j["ActionSequnceId"] =  MongoDB_Bridge::currentActionSequenceId;

    j["SimulatedState"] = json::parse(Prints::GetStateJson(*state)); 
    
    
    std::string str(j.dump().c_str());
    MongoDB_Bridge::SaveSimulatedState(str);
}
        }
