
#include <despot/model_primitives/icaps/actionManager.h>
#include <despot/util/mongoDB_Bridge.h>
#include <nlohmann/json.hpp> 

// for convenience
using json = nlohmann::json;
//#include "actionManager.h"
#include <vector>
#include <utility>
#include <string>
namespace despot { 
    void ActionDescription::SetActionParametersByState(IcapsState *state, std::vector<std::string> indexes){}
    std::vector<ActionDescription*> ActionManager::actions;


void NavigateActionDescription::SetActionParametersByState(IcapsState *state, std::vector<std::string> indexes)
{
    strLink_oDesiredLocation = indexes[0];
    oDesiredLocation = (state->tLocationObjectsForActions[indexes[0]]);
}
std::string NavigateActionDescription::GetActionParametersJson_ForActionExecution()
{  
    json j;
    j["ParameterLinks"]["oDesiredLocation"] = strLink_oDesiredLocation;
    j["ParameterValues"]["oDesiredLocation"]["discrete_location"] = oDesiredLocation.discrete_location;
    j["ParameterValues"]["oDesiredLocation"]["actual_location"] = oDesiredLocation.actual_location;

    std::string str(j.dump().c_str());
    return str;
}
std::string NavigateActionDescription::GetActionParametersJson_ForActionRegistration()
{
    json j;
    j["oDesiredLocation->discrete_location"] = oDesiredLocation.discrete_location;
    j["oDesiredLocation->actual_location"] = oDesiredLocation.actual_location;

    std::string str(j.dump().c_str());
    return str;
}

void ActionManager::Init(IcapsState* state)
{
	
	int id = 0;
    ActionDescription *place = new ActionDescription;
    place->actionType = placeAction;
    place->actionId = id++;
    ActionManager::actions.push_back(place);

    ActionDescription *observe = new ActionDescription;
    observe->actionType = observeAction;
    observe->actionId = id++;
    ActionManager::actions.push_back(observe);

    ActionDescription *pick = new ActionDescription;
    pick->actionType = pickAction;
    pick->actionId = id++;
    ActionManager::actions.push_back(pick);

    NavigateActionDescription* navigateActions = new NavigateActionDescription[4];
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
        MongoDB_Bridge::RegisterAction(ActionManager::actions[j]->actionId, enum_map_icaps::vecActionTypeEnumToString[ActionManager::actions[j]->actionType], ActionManager::actions[j]->GetActionParametersJson_ForActionRegistration(), actDesc);
    }
}


 
    std::string Prints::PrinttDiscreteLocation(tDiscreteLocation enumT)
    {
        switch (enumT)
        {
            case eOutside_lab211:
                return "eOutside_lab211";
            case eAuditorium:
                return "eAuditorium";
            case eCorridor:
                return "eCorridor";
            case eNear_elevator1:
                return "eNear_elevator1";
            case eUnknown:
                return "eUnknown";
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
            ss << "," << "discrete_location:" << Prints::PrinttDiscreteLocation((tDiscreteLocation)navigateA->oDesiredLocation.discrete_location);;
            ss << "," << "actual_location:" << navigateA->oDesiredLocation.actual_location;
        }

        return ss.str();
    }


std::string Prints::PrintObs(int action, int obs)
{
	IcapsResponseModuleAndTempEnums eObs = (IcapsResponseModuleAndTempEnums)obs;
	return enum_map_icaps::vecResponseEnumToString[eObs]; 
}
    std::string Prints::PrintState(IcapsState state)
    {
        stringstream ss;
        ss << "STATE: ";
        ss << "|cupDiscreteGeneralLocation:";
        ss <<  Prints::PrinttDiscreteLocation(state.cupDiscreteGeneralLocation);
        ss << "|cupAccurateLocation:";
        ss <<  state.cupAccurateLocation;
        ss << "|handEmpty:";
        ss <<  state.handEmpty;
        ss << "|robotGenerallocation:";
        ss <<  Prints::PrinttDiscreteLocation(state.robotGenerallocation);
        return ss.str();
    }


 
    std::string Prints::PrintActionType(ActionType actType)
    {
        switch (actType)
        {
        case placeAction:
            return "placeAction";
        case observeAction:
            return "observeAction";
        case pickAction:
            return "pickAction";
        case navigateAction:
            return "navigateAction";
        }
    }

std::string Prints::GetStateJson(State& _state)
    {
        const IcapsState& state = static_cast<const IcapsState&>(_state);
        json j;
        j["cupDiscreteGeneralLocation"] = state.cupDiscreteGeneralLocation;
        j["cupAccurateLocation"] = state.cupAccurateLocation;
        j["handEmpty"] = state.handEmpty;
        j["robotGenerallocation"] = state.robotGenerallocation;
        j["locationOutside_lab211"]["discrete_location"] = state.locationOutside_lab211.discrete_location;
        j["locationOutside_lab211"]["actual_location"] = state.locationOutside_lab211.actual_location;
        j["locationAuditorium"]["discrete_location"] = state.locationAuditorium.discrete_location;
        j["locationAuditorium"]["actual_location"] = state.locationAuditorium.actual_location;
        j["locationNear_elevator1"]["discrete_location"] = state.locationNear_elevator1.discrete_location;
        j["locationNear_elevator1"]["actual_location"] = state.locationNear_elevator1.actual_location;
        j["locationCorridor"]["discrete_location"] = state.locationCorridor.discrete_location;
        j["locationCorridor"]["actual_location"] = state.locationCorridor.actual_location;

    std::string str(j.dump().c_str());
    return str;
     
    }

    void Prints::GetStateFromJson(IcapsState& state, std::string jsonStr, int stateIndex)
    {
        
        json j = json::parse(jsonStr);
        j = j["BeliefeState"];

        state.cupDiscreteGeneralLocation = j[stateIndex]["cupDiscreteGeneralLocation"];
        state.cupAccurateLocation = j[stateIndex]["cupAccurateLocation"];
        state.handEmpty = j[stateIndex]["handEmpty"];
        state.robotGenerallocation = j[stateIndex]["robotGenerallocation"];
        state.locationOutside_lab211.discrete_location = j[stateIndex]["locationOutside_lab211"]["discrete_location"];
        state.locationOutside_lab211.actual_location = j[stateIndex]["locationOutside_lab211"]["actual_location"];
        state.locationAuditorium.discrete_location = j[stateIndex]["locationAuditorium"]["discrete_location"];
        state.locationAuditorium.actual_location = j[stateIndex]["locationAuditorium"]["actual_location"];
        state.locationNear_elevator1.discrete_location = j[stateIndex]["locationNear_elevator1"]["discrete_location"];
        state.locationNear_elevator1.actual_location = j[stateIndex]["locationNear_elevator1"]["actual_location"];
        state.locationCorridor.discrete_location = j[stateIndex]["locationCorridor"]["discrete_location"];
        state.locationCorridor.actual_location = j[stateIndex]["locationCorridor"]["actual_location"];

    }



void Prints::SaveBeliefParticles(vector<State*> particles)
{
    json j;
    j["ActionSequnceId"] =  MongoDB_Bridge::currentActionSequenceId;

    for (int i = 0; i < particles.size(); i++)
    {
        j["BeliefeState"][i] = json::parse(Prints::GetStateJson(*particles[0])); 
    }
    
    std::string str(j.dump().c_str());

    j["ActionSequnceId"] = -1;

    std::string currentBeliefStr(j.dump().c_str());
    MongoDB_Bridge::SaveBeliefState(str, currentBeliefStr);
}
}