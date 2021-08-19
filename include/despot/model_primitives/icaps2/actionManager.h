#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

#include "state.h"
#include <despot/model_primitives/icaps2/enum_map_icaps2.h> 
#include <vector>
#include <utility>
#include <string>
namespace despot { 


    class ActionDescription
    {
    public:
        int actionId;
        ActionType actionType;
        virtual void SetActionParametersByState(Icaps2State *state, std::vector<std::string> indexes);
        virtual std::string GetActionParametersJson_ForActionExecution() { return ""; };
        virtual std::string GetActionParametersJson_ForActionRegistration() { return ""; };
        
    };

class NavigateActionDescription: public ActionDescription
{
    public:
        tLocation oDesiredLocation;
        std::string strLink_oDesiredLocation;
        std::string strDescription_oDesiredLocation;
        NavigateActionDescription(int _oDesiredLocation_Index);
        virtual void SetActionParametersByState(Icaps2State *state, std::vector<std::string> indexes);
        virtual std::string GetActionParametersJson_ForActionExecution();
        virtual std::string GetActionParametersJson_ForActionRegistration();
        NavigateActionDescription(){};
        NavigateActionDescription(tLocation _oDesiredLocation, 
            std::string _strLink_oDesiredLocation, 
            std::string _strDescription_oDesiredLocation):oDesiredLocation(_oDesiredLocation), strLink_oDesiredLocation(_strLink_oDesiredLocation), strDescription_oDesiredLocation(_strDescription_oDesiredLocation){};
};



class ActionManager {
public:
	static std::vector<ActionDescription*> actions;
    static void Init(Icaps2State* state);
};
}
#endif //ACTION_MANAGER_H
