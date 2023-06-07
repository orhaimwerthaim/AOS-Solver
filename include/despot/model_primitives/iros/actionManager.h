#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

<<<<<<<< HEAD:include/despot/model_primitives/iros/actionManager.h
#include "enum_map_iros.h" 
========
#include "enum_map_collectValuableToys.h" 
>>>>>>>> origin/master:include/despot/model_primitives/collectValuableToys/actionManager.h
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

class Detect_board_stateActionDescription: public ActionDescription
{
    public:
<<<<<<<< HEAD:include/despot/model_primitives/iros/actionManager.h
        //Detect_board_stateActionDescription();
========
        int oDestination;
        std::string strLink_oDestination;
        NavigateActionDescription(int _oDestination_Index);
>>>>>>>> origin/master:include/despot/model_primitives/collectValuableToys/actionManager.h
        virtual void SetActionParametersByState(State *state, std::vector<std::string> indexes);

                        
        virtual std::string GetActionParametersJson_ForActionExecution();
        virtual std::string GetActionParametersJson_ForActionRegistration();
        Detect_board_stateActionDescription(){};
};

class Draw_in_cellActionDescription: public ActionDescription
{
    public:
        int oCellP;
        std::string strLink_oCellP;
        Draw_in_cellActionDescription(int _oCellP_Index);
        virtual void SetActionParametersByState(State *state, std::vector<std::string> indexes);
        virtual void SetActionParametersByState(std::tuple<int> parameters);

                        
        virtual std::string GetActionParametersJson_ForActionExecution();
        virtual std::string GetActionParametersJson_ForActionRegistration();
        Draw_in_cellActionDescription(){};
};

class PickActionDescription: public ActionDescription
{
    public:
        string toyType;
        std::string strLink_toyType;
        PickActionDescription(int _toyType_Index);
        virtual void SetActionParametersByState(State *state, std::vector<std::string> indexes);

                        
        virtual std::string GetActionParametersJson_ForActionExecution();
        virtual std::string GetActionParametersJson_ForActionRegistration();
        PickActionDescription(){};
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



class ActionManager {
public:
	static std::vector<ActionDescription*> actions;
    static void Init(State* state);
};


class Prints
{
	public:
<<<<<<<< HEAD:include/despot/model_primitives/iros/actionManager.h
    static std::string PrinttSymbols(tSymbols);
========
>>>>>>>> origin/master:include/despot/model_primitives/collectValuableToys/actionManager.h

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
