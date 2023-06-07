
#include <despot/model_primitives/iros/actionManager.h>
#include <despot/util/mongoDB_Bridge.h>
#include <nlohmann/json.hpp> 

// for convenience
using json = nlohmann::json;
//#include "actionManager.h"
#include <vector>
#include <utility>
#include <string>
namespace despot { 
    
    void ActionDescription::SetActionParametersByState(IrosState *state, std::vector<std::string> indexes){}
    std::vector<ActionDescription*> ActionManager::actions;


    void Draw_in_cellActionDescription::SetActionParametersByState(std::tuple<int> parameters)
    {
        strLink_oCellP = "not applicable in this AOS version";
        oCellP = std::get<0>(parameters);
        
    }
void Draw_in_cellActionDescription::SetActionParametersByState(IrosState *state, std::vector<std::string> indexes)
{
}
std::string Draw_in_cellActionDescription::GetActionParametersJson_ForActionExecution()
{  
    json j;
    j["ParameterLinks"]["oCellP"] = strLink_oCellP;
    j["ParameterValues"]["oCellP"] = oCellP;

    std::string str(j.dump().c_str());
    return str;
}
std::string Draw_in_cellActionDescription::GetActionParametersJson_ForActionRegistration()
{
    json j;
    j["oCellP"] = oCellP;

    std::string str(j.dump().c_str());
    return str;
}

void ActionManager::Init(IrosState* state)
{
	
	int id = 0;
    ActionDescription *detect_board_state = new ActionDescription;
    detect_board_state->actionType = detect_board_stateAction;
    detect_board_state->actionId = id++;
    ActionManager::actions.push_back(detect_board_state);

        vector<tuple<int>> draw_in_cellParameters{};
        auto draw_in_cellGetParameters = [&]()
        {
            vector<tuple<int>> __possibleParameters{};
            
            for(int i=0;
            i<9;
            i++)
{
  
  __possibleParameters.push_back(std::make_tuple(i));
            
};
            return __possibleParameters;
            
        };
        draw_in_cellParameters = draw_in_cellGetParameters();
        Draw_in_cellActionDescription* draw_in_cellActions = new Draw_in_cellActionDescription[draw_in_cellParameters.size()];
        for(int i=0;i< draw_in_cellParameters.size();i++)
        {
            Draw_in_cellActionDescription &oDraw_in_cellAction = draw_in_cellActions[i];
            oDraw_in_cellAction.SetActionParametersByState(draw_in_cellParameters[i]);
            oDraw_in_cellAction.actionId = id++;
            oDraw_in_cellAction.actionType = draw_in_cellAction;
            ActionManager::actions.push_back(&oDraw_in_cellAction);
        }


    for(int j=0;j< ActionManager::actions.size();j++)
    {
        std::string actDesc = Prints::PrintActionDescription(ActionManager::actions[j]);
        MongoDB_Bridge::RegisterAction(ActionManager::actions[j]->actionId, enum_map_iros::vecActionTypeEnumToString[ActionManager::actions[j]->actionType], ActionManager::actions[j]->GetActionParametersJson_ForActionRegistration(), actDesc);
    }
    
}
std::string Prints::PrintActionDescription(int actionId)
{
    return Prints::PrintActionDescription(ActionManager::actions[actionId]);
}


 
    std::string Prints::PrinttSymbols(tSymbols enumT)
    {
        switch (enumT)
        {
            case eEmpty:
                return "eEmpty";
            case eO:
                return "eO";
            case eX:
                return "eX";
        }
        return "";

    }

    std::string Prints::PrintActionDescription(ActionDescription* act)
    {
        stringstream ss;
        ss << "ID:" << act->actionId;
        ss << "," << PrintActionType(act->actionType);
        if(act->actionType == draw_in_cellAction)
        {
            Draw_in_cellActionDescription *draw_in_cellA = static_cast<Draw_in_cellActionDescription *>(act);
            ss << "," << "oCellP:" << draw_in_cellA->oCellP;
        }

        return ss.str();
    }



    std::string Prints::PrintState(IrosState state)
    {
        stringstream ss;
        ss << "STATE: " << endl;
        ss << "|state.grid:";
        ss << state.grid;
        ss << "|state.isRobotTurn:";
        ss << state.isRobotTurn;
        return ss.str();
    }


 
    std::string Prints::PrintActionType(ActionType actType)
    {
        switch (actType)
        {
        case detect_board_stateAction:
            return "detect_board_stateAction";
        case draw_in_cellAction:
            return "draw_in_cellAction";
        }
        return "";
    }

std::string Prints::GetStateJson(State& _state)
    {
        const IrosState& state = static_cast<const IrosState&>(_state);
        json j;
        json jgrid(state.grid);
        j["grid"] = jgrid;
        j["isRobotTurn"] = state.isRobotTurn;

    std::string str(j.dump().c_str());
    return str;
     
    }

    void Prints::GetStateFromJson(IrosState& state, std::string jsonStr, int stateIndex)
    {
        
        json j = json::parse(jsonStr);
        j = j["BeliefeState"];

        state.grid = j[stateIndex]["grid"].get<std::vector<int>>();
        state.isRobotTurn = j[stateIndex]["isRobotTurn"];

    }



std::string Prints::PrintObs(int obs)
{
	IrosResponseModuleAndTempEnums eObs = (IrosResponseModuleAndTempEnums)obs;
	return enum_map_iros::vecResponseEnumToString[eObs]; 
}

void Prints::SaveBeliefParticles(vector<State*> particles)
{
    json j;
    j["ActionSequnceId"] =  MongoDB_Bridge::currentActionSequenceId;

    for (int i = 0; i < particles.size(); i++)
    {
        j["BeliefeState"][i] = json::parse(Prints::GetStateJson(*particles[i])); 
    }
    
    std::string str(j.dump().c_str());

    j["ActionSequnceId"] = -1;

    std::string currentBeliefStr(j.dump().c_str());
    MongoDB_Bridge::SaveBeliefState(str, currentBeliefStr);
}



void Prints::SaveSimulatedState(State* state)
{
    json j;
    j["ActionSequnceId"] =  MongoDB_Bridge::currentActionSequenceId;

    j["SimulatedState"] = json::parse(Prints::GetStateJson(*state)); 
    
    
    std::string str(j.dump().c_str());
    MongoDB_Bridge::SaveSimulatedState(str);
}
        }
