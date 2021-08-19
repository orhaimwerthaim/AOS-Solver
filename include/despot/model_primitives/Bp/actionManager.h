#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

#include "state.h"
#include <despot/model_primitives/Bp/enum_map_Bp.h> 
#include <vector>
#include <utility>
#include <string> 
namespace despot { 


    class ActionDescription
    {
    public:
        int actionId;
        ActionType actionType;
        virtual void SetActionParametersByState(BpState *state, std::vector<std::string> indexes);
        virtual std::string GetActionParametersJson_ForActionExecution() { return ""; };
        virtual std::string GetActionParametersJson_ForActionRegistration() { return ""; };
        
    };

class NavigateActionDescription: public ActionDescription
{
    public:
        tDirection oDirection;
        std::string strLink_oDirection;
        NavigateActionDescription(int _oDirection_Index);
        virtual void SetActionParametersByState(BpState *state, std::vector<std::string> indexes);
        virtual std::string GetActionParametersJson_ForActionExecution();
        virtual std::string GetActionParametersJson_ForActionRegistration();
        NavigateActionDescription(){};
};



class ActionManager {
public:
	static std::vector<ActionDescription*> actions;
    static void Init(BpState* state);
};


class Prints
{
	public:
    static std::string PrinttDirection(tDirection);
    static std::string PrinttCell(tCell);

	static std::string PrintActionDescription(ActionDescription*);
	static std::string PrintActionType(ActionType);
	static std::string PrintState(BpState state);
	static std::string PrintObs(int action, int obs);
    static std::string GetJsonForBelief(vector<State*> particles, int actionSequenceId);
    static std::string GetStateJson(State& state);
};
}
#endif //ACTION_MANAGER_H
