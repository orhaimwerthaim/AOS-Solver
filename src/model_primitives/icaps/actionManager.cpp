
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
    //std::string s= utility::conversions::to_utf8string(j.dump(4));
}

std::string NavigateActionDescription::GetActionParametersJson_ForActionRegistration()
{
    json j;
    j["oDesiredLocation->discrete_location"] = oDesiredLocation.discrete_location;
    std::string str(j.dump().c_str());
    return str;
}

void ActionManager::Init(IcapsState* state)
{
	
	int id = 0;
	ActionDescription *pick = new ActionDescription;
	pick->actionType = pickAction;
	pick->actionId = id++;
	ActionManager::actions.push_back(pick);

	ActionDescription *place = new ActionDescription;
    place->actionType = placeAction;
	place->actionId = id++;
    ActionManager::actions.push_back(place);

	ActionDescription *observe = new ActionDescription;
    observe->actionType = observeAction;
	observe->actionId = id++;
    ActionManager::actions.push_back(observe);
	
	NavigateActionDescription* navActions = new NavigateActionDescription[4];
    std::vector<std::string> indexes;

    map<std::string, tLocation>::iterator it;
    int i = 0;
    for (it = state->tLocationObjectsForActions.begin(); it != state->tLocationObjectsForActions.end(); it++)
    {
        indexes.clear();
        indexes.push_back(it->first);
        NavigateActionDescription &navAction = navActions[i];
        navAction.SetActionParametersByState(state, indexes);
		navAction.actionId = id++;
        navAction.actionType = navigateAction;
        ActionManager::actions.push_back(&navAction);
        i++;
    }
    // for (int i = 0; i < 4; i++)
    // {
    //     indexes.clear();
    //     indexes.push_back(i);
    //     NavigateActionDescription &navAction = navActions[i];
    //     navAction.SetActionParametersByState(state, indexes);
	// 	navAction.actionId = id++;
    //     navAction.actionType = navigateAction;
    //     ActionManager::actions.push_back(&navAction);
    // }

	for(int j=0;j< ActionManager::actions.size();j++)
	{
        MongoDB_Bridge::RegisterAction(ActionManager::actions[j]->actionId, enum_map_icaps::vecActionTypeEnumToString[ActionManager::actions[j]->actionType], ActionManager::actions[j]->GetActionParametersJson_ForActionRegistration());
        logd << "actionID:" << ActionManager::actions[j]->actionId << ", type:" << ActionManager::actions[j]->actionType << ", index:" << ((j < 3) ? 1 : (*(static_cast<NavigateActionDescription *>(ActionManager::actions[j]))).oDesiredLocation.discrete_location);
    }
}
}