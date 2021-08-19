
#include <despot/model_primitives/icaps3/actionManager.h>
#include <despot/util/mongoDB_Bridge.h>
#include <nlohmann/json.hpp> 

// for convenience
using json = nlohmann::json;
//#include "actionManager.h"
#include <vector>
#include <utility>
#include <string>
namespace despot { 
    void ActionDescription::SetActionParametersByState(Icaps3State *state, std::vector<std::string> indexes){}
    std::vector<ActionDescription*> ActionManager::actions;


void NavigateActionDescription::SetActionParametersByState(Icaps3State *state, std::vector<std::string> indexes)
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

void ActionManager::Init(Icaps3State* state)
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
    std::vector<std::string> indexes;
    int i = 0;
    map<std::string, tLocation>::iterator it1;
    for (it1 = state->tLocationObjectsForActions.begin(); it1 != state->tLocationObjectsForActions.end(); it1++)
    {
        indexes.push_back(it1->first);
        NavigateActionDescription &oNavigateAction = navigateActions[i];
        oNavigateAction.SetActionParametersByState(state, indexes);
        oNavigateAction.actionId = id++;
        oNavigateAction.actionType = navigateAction;
        ActionManager::actions.push_back(&oNavigateAction);
        i++;
        indexes.pop_back();
    }


	for(int j=0;j< ActionManager::actions.size();j++)
	{
        MongoDB_Bridge::RegisterAction(ActionManager::actions[j]->actionId, enum_map_icaps3::vecActionTypeEnumToString[ActionManager::actions[j]->actionType], ActionManager::actions[j]->GetActionParametersJson_ForActionRegistration());
    }
}
}