#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

#include "state.h"
#include <despot/model_primitives/icaps/enum_map_icaps.h> 
#include <vector>
#include <utility>
#include <string>
namespace despot { 


    class ActionDescription
    {
    public:
        int actionId;
        ActionType actionType;
        virtual void SetActionParametersByState(IcapsState *state);
};

class NavigateActionDescription: public ActionDescription
{
    public: 
        tLocation oDesiredLocation;
        NavigateActionDescription(int _oDesiredLocation_Index);
        virtual void SetActionParametersByState(IcapsState *state);
    private:
        int oDesiredLocation_Index;
};

class ActionManager {
public:
	static std::vector<ActionDescription> actions;
    ActionManager();
};


}
#endif //ACTION_MANAGER_H
