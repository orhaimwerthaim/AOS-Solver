#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

#include "enum_map_collect_toys.h" 
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

class DetectActionDescription: public ActionDescription
{
    public:
        //DetectActionDescription();
        virtual void SetActionParametersByState(State *state, std::vector<std::string> indexes);

                        
        virtual std::string GetActionParametersJson_ForActionExecution();
        virtual std::string GetActionParametersJson_ForActionRegistration();
        DetectActionDescription(){};
};

class PlaceActionDescription: public ActionDescription
{
    public:
        //PlaceActionDescription();
        virtual void SetActionParametersByState(State *state, std::vector<std::string> indexes);

                        
        virtual std::string GetActionParametersJson_ForActionExecution();
        virtual std::string GetActionParametersJson_ForActionRegistration();
        PlaceActionDescription(){};
};

class PickActionDescription: public ActionDescription
{
    public:
        string toyType;
        std::string strLink_toyType;
        PickActionDescription(int _toyType_Index);
        virtual void SetActionParametersByState(State *state, std::vector<std::string> indexes);
        virtual void SetActionParametersByState(std::tuple<string> parameters);

                        
        virtual std::string GetActionParametersJson_ForActionExecution();
        virtual std::string GetActionParametersJson_ForActionRegistration();
        PickActionDescription(){};
};

class NavigateActionDescription: public ActionDescription
{
    public:
        int oDestination;
        std::string strLink_oDestination;
        NavigateActionDescription(int _oDestination_Index);
        virtual void SetActionParametersByState(State *state, std::vector<std::string> indexes);
        virtual void SetActionParametersByState(std::tuple<int> parameters);

                        
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
    static std::string PrinteLocation(eLocation);

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
