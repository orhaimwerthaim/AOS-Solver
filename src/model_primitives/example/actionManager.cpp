
#include <despot/model_primitives/example/actionManager.h>
#include <despot/util/mongoDB_Bridge.h>
#include <nlohmann/json.hpp> 

// for convenience
using json = nlohmann::json;
//#include "actionManager.h"
#include <vector>
#include <utility>
#include <string>
namespace despot { 
    
    void ActionDescription::SetActionParametersByState(ExampleState *state, std::vector<std::string> indexes){}
    std::vector<ActionDescription*> ActionManager::actions;


void NavigateActionDescription::SetActionParametersByState(ExampleState *state, std::vector<std::string> indexes)
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
    j["ParameterValues"]["oDesiredLocation"]["discrete"] = oDesiredLocation.discrete;

    std::string str(j.dump().c_str());
    return str;
}
std::string NavigateActionDescription::GetActionParametersJson_ForActionRegistration()
{
    json j;
    j["oDesiredLocation->x"] = oDesiredLocation.x;
    j["oDesiredLocation->y"] = oDesiredLocation.y;
    j["oDesiredLocation->z"] = oDesiredLocation.z;
    j["oDesiredLocation->discrete"] = oDesiredLocation.discrete;

    std::string str(j.dump().c_str());
    return str;
}

void ActionManager::Init(ExampleState* state)
{
	
	int id = 0;
    NavigateActionDescription* navigateActions = new NavigateActionDescription[3];
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
        MongoDB_Bridge::RegisterAction(ActionManager::actions[j]->actionId, enum_map_example::vecActionTypeEnumToString[ActionManager::actions[j]->actionType], ActionManager::actions[j]->GetActionParametersJson_ForActionRegistration(), actDesc);
    }
    
}

int Prints::GetHash(string str_)
    {
        const char *str = str_.c_str();
        unsigned long hash = 0;
        int c;

        while (c = *str++)
            hash = c + (hash << 6) + (hash << 16) - hash;

        return hash; 
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
            ss << "," << "x:" << navigateA->oDesiredLocation.x;
            ss << "," << "y:" << navigateA->oDesiredLocation.y;
            ss << "," << "z:" << navigateA->oDesiredLocation.z;
            ss << "," << "discrete:" << navigateA->oDesiredLocation.discrete;
        }

        return ss.str();
    }



    std::string Prints::PrintState(ExampleState state)
    {
        stringstream ss;
        ss << "STATE: " << endl;
        ss << "|state.robotLocation.x:";
        ss << state.robotLocation.x;
        ss << "|state.robotLocation.y:";
        ss << state.robotLocation.y;
        ss << "|state.robotLocation.z:";
        ss << state.robotLocation.z;
        ss << "|state.robotLocation.discrete:";
        ss << state.robotLocation.discrete;
        ss << "|state.v1.discrete:";
        ss << state.v1.discrete;
        ss << "|state.v1.visited:";
        ss << state.v1.visited;
        ss << "|state.v2.discrete:";
        ss << state.v2.discrete;
        ss << "|state.v2.visited:";
        ss << state.v2.visited;
        ss << "|state.v3.discrete:";
        ss << state.v3.discrete;
        ss << "|state.v3.visited:";
        ss << state.v3.visited;
        return ss.str();
    }


 
    std::string Prints::PrintActionType(ActionType actType)
    {
        switch (actType)
        {
        case navigateAction:
            return "navigateAction";
        }
        return "";
    }

std::string Prints::GetStateJson(State& _state)
    {
        const ExampleState& state = static_cast<const ExampleState&>(_state);
        json j;
        j["robotLocation"]["x"] = state.robotLocation.x;
        j["robotLocation"]["y"] = state.robotLocation.y;
        j["robotLocation"]["z"] = state.robotLocation.z;
        j["robotLocation"]["discrete"] = state.robotLocation.discrete;
        j["v1"]["discrete"] = state.v1.discrete;
        j["v1"]["visited"] = state.v1.visited;
        j["v2"]["discrete"] = state.v2.discrete;
        j["v2"]["visited"] = state.v2.visited;
        j["v3"]["discrete"] = state.v3.discrete;
        j["v3"]["visited"] = state.v3.visited;
        j["l1"]["x"] = state.l1.x;
        j["l1"]["y"] = state.l1.y;
        j["l1"]["z"] = state.l1.z;
        j["l1"]["discrete"] = state.l1.discrete;
        j["l2"]["x"] = state.l2.x;
        j["l2"]["y"] = state.l2.y;
        j["l2"]["z"] = state.l2.z;
        j["l2"]["discrete"] = state.l2.discrete;
        j["l3"]["x"] = state.l3.x;
        j["l3"]["y"] = state.l3.y;
        j["l3"]["z"] = state.l3.z;
        j["l3"]["discrete"] = state.l3.discrete;

    std::string str(j.dump().c_str());
    return str;
     
    }

    void Prints::GetStateFromJson(ExampleState& state, std::string jsonStr, int stateIndex)
    {
        
        json j = json::parse(jsonStr);
        j = j["BeliefeState"];

        state.robotLocation.x = j[stateIndex]["robotLocation"]["x"];
        state.robotLocation.y = j[stateIndex]["robotLocation"]["y"];
        state.robotLocation.z = j[stateIndex]["robotLocation"]["z"];
        state.robotLocation.discrete = j[stateIndex]["robotLocation"]["discrete"];
        state.v1.discrete = j[stateIndex]["v1"]["discrete"];
        state.v1.visited = j[stateIndex]["v1"]["visited"];
        state.v2.discrete = j[stateIndex]["v2"]["discrete"];
        state.v2.visited = j[stateIndex]["v2"]["visited"];
        state.v3.discrete = j[stateIndex]["v3"]["discrete"];
        state.v3.visited = j[stateIndex]["v3"]["visited"];
        state.l1.x = j[stateIndex]["l1"]["x"];
        state.l1.y = j[stateIndex]["l1"]["y"];
        state.l1.z = j[stateIndex]["l1"]["z"];
        state.l1.discrete = j[stateIndex]["l1"]["discrete"];
        state.l2.x = j[stateIndex]["l2"]["x"];
        state.l2.y = j[stateIndex]["l2"]["y"];
        state.l2.z = j[stateIndex]["l2"]["z"];
        state.l2.discrete = j[stateIndex]["l2"]["discrete"];
        state.l3.x = j[stateIndex]["l3"]["x"];
        state.l3.y = j[stateIndex]["l3"]["y"];
        state.l3.z = j[stateIndex]["l3"]["z"];
        state.l3.discrete = j[stateIndex]["l3"]["discrete"];

    }



std::string Prints::PrintObs(int obs)
{
	ExampleResponseModuleAndTempEnums eObs = (ExampleResponseModuleAndTempEnums)obs;
	return enum_map_example::vecResponseEnumToString[eObs]; 
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
