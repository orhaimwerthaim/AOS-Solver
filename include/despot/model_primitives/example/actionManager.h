#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

#include "enum_map_example.h" 
#include <despot/core/pomdp.h>
#include "state_var_types.h"
#include <vector>
#include <utility>
#include <string>
namespace despot { 


    class ActionDescription
    {
    public:
        int actionId;
        ActionType actionType;
        virtual void SetActionParametersByState(State *state, std::vector<std::string> indexes);
        virtual std::string GetActionParametersJson_ForActionExecution() { return ""; };
        virtual std::string GetActionParametersJson_ForActionRegistration() { return ""; };
        
    };

class NavigateActionDescription: public ActionDescription
{
    public:
        tLocation oDesiredLocation;
        std::string strLink_oDesiredLocation;
        NavigateActionDescription(int _oDesiredLocation_Index);
        virtual void SetActionParametersByState(State *state, std::vector<std::string> indexes);

                        
        virtual std::string GetActionParametersJson_ForActionExecution();
        virtual std::string GetActionParametersJson_ForActionRegistration();
        NavigateActionDescription(){};
};



class ActionManager {
public:
	static std::vector<ActionDescription*> actions;
    static void Init(State* state);
};


class Prints
{
	public:
    static int GetHash(string s);

	static std::string PrintActionDescription(ActionDescription*);
    static std::string PrintActionDescription(int actionId);
	static std::string PrintActionType(ActionType);
	static std::string PrintState(State state);
	static std::string PrintObs(int obs);
    static void SaveBeliefParticles(vector<State *> particles);
    static void SaveSimulatedState(State* state);
    static std::string GetStateJson(State &state);
    static void GetStateFromJson(State &state, std::string jsonStr, int stateIndex);
};

}
#endif //ACTION_MANAGER_H
