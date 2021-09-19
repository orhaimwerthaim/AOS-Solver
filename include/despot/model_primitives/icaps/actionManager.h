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
        virtual void SetActionParametersByState(IcapsState *state, std::vector<std::string> indexes);
        virtual std::string GetActionParametersJson_ForActionExecution() { return ""; };
        virtual std::string GetActionParametersJson_ForActionRegistration() { return ""; };
        
    };

class NavigateActionDescription: public ActionDescription
{
    public:
        tLocation oDesiredLocation;
        std::string strLink_oDesiredLocation;
        NavigateActionDescription(int _oDesiredLocation_Index);
        virtual void SetActionParametersByState(IcapsState *state, std::vector<std::string> indexes);
        virtual std::string GetActionParametersJson_ForActionExecution();
        virtual std::string GetActionParametersJson_ForActionRegistration();
        NavigateActionDescription(){};
};



class ActionManager {
public:
	static std::vector<ActionDescription*> actions;
    static void Init(IcapsState* state);
};


class Prints
{
	public:
    static std::string PrinttDiscreteLocation(tDiscreteLocation);

	static std::string PrintActionDescription(ActionDescription*);
	static std::string PrintActionType(ActionType);
	static std::string PrintState(IcapsState state);
	static std::string PrintObs(int action, int obs);
    static void SaveBeliefParticles(vector<State *> particles);
    static std::string GetStateJson(State &state);
    static void GetStateFromJson(IcapsState &state, std::string jsonStr, int stateIndex);
};
}
#endif //ACTION_MANAGER_H
