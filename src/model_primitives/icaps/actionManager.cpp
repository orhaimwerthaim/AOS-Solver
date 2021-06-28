
#include <despot/model_primitives/icaps/actionManager.h>
//#include "actionManager.h"
#include <vector>
#include <utility>
#include <string>
namespace despot { 
    void ActionDescription::SetActionParametersByState(IcapsState *state){}
    std::vector<ActionDescription> ActionManager::actions;

    NavigateActionDescription::NavigateActionDescription(int _oDesiredLocation_Index)
    {
        oDesiredLocation_Index = _oDesiredLocation_Index;
}

void NavigateActionDescription::SetActionParametersByState(IcapsState *state)
{
    oDesiredLocation = state->tLocationObjects[oDesiredLocation_Index];
}

ActionManager::ActionManager()
{
    ActionDescription pick;
    pick.actionType = pickAction;
    ActionManager::actions.push_back(pick);

    ActionDescription place;
    pick.actionType = pickAction;
    ActionManager::actions.push_back(place);

    for (int i = 0; i < 5;i++)
    {
        NavigateActionDescription navAction = NavigateActionDescription(i);
        ActionManager::actions.push_back(navAction);
    }
}
}