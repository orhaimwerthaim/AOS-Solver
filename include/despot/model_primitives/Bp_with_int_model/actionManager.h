#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

#include "state.h"
#include <despot/model_primitives/Bp_with_int_model/enum_map_Bp_with_int_model.h> 
#include <vector>
#include <utility>
#include <string>
namespace despot { 


    class ActionDescription
    {
    public:
        int actionId;
        ActionType actionType;
        virtual void SetActionParametersByState(Bp_with_int_modelState *state, std::vector<std::string> indexes);
        virtual std::string GetActionParametersJson_ForActionExecution() { return ""; };
        virtual std::string GetActionParametersJson_ForActionRegistration() { return ""; };
        
    };

class PushActionDescription: public ActionDescription
{
    public:
        tDirection oDirection;
        tPushType oIsJointPush;
        std::string strLink_oDirection;
        std::string strLink_oIsJointPush;
        PushActionDescription(int _oDirection_Index, int _oIsJointPush_Index);
        virtual void SetActionParametersByState(Bp_with_int_modelState *state, std::vector<std::string> indexes);
        virtual std::string GetActionParametersJson_ForActionExecution();
        virtual std::string GetActionParametersJson_ForActionRegistration();
        PushActionDescription(){};
};

class NavigateActionDescription: public ActionDescription
{
    public:
        tDirection oDirection;
        std::string strLink_oDirection;
        NavigateActionDescription(int _oDirection_Index);
        virtual void SetActionParametersByState(Bp_with_int_modelState *state, std::vector<std::string> indexes);
        virtual std::string GetActionParametersJson_ForActionExecution();
        virtual std::string GetActionParametersJson_ForActionRegistration();
        NavigateActionDescription(){};
};



class ActionManager {
public:
	static std::vector<ActionDescription*> actions;
    static void Init(Bp_with_int_modelState* state);
};


class Prints
{
	public:
    static std::string PrinttPushType(tPushType);
    static std::string PrinttDirection(tDirection);

	static std::string PrintActionDescription(ActionDescription*);
	static std::string PrintActionType(ActionType);
	static std::string PrintState(Bp_with_int_modelState state);
	static std::string PrintObs(int action, int obs);
    static void SaveBeliefParticles(vector<State *> particles);
    static std::string GetStateJson(State &state);
    static void GetStateFromJson(Bp_with_int_modelState &state, std::string jsonStr, int stateIndex);
};
}
#endif //ACTION_MANAGER_H
