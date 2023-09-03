
#include <despot/model_primitives/collect_toys/actionManager.h>
#include <despot/util/mongoDB_Bridge.h>
#include <nlohmann/json.hpp> 

// for convenience
using json = nlohmann::json;
//#include "actionManager.h"
#include <vector>
#include <utility>
#include <string>
namespace despot { 
    
    void ActionDescription::SetActionParametersByState(Collect_toysState *state, std::vector<std::string> indexes){}
    std::vector<ActionDescription*> ActionManager::actions;


    void PickActionDescription::SetActionParametersByState(std::tuple<string> parameters)
    {
        strLink_toyType = "not applicable in this AOS version";
        toyType = std::get<0>(parameters);
        
    }
void PickActionDescription::SetActionParametersByState(Collect_toysState *state, std::vector<std::string> indexes)
{
}
std::string PickActionDescription::GetActionParametersJson_ForActionExecution()
{  
    json j;
    j["ParameterLinks"]["toyType"] = strLink_toyType;
    j["ParameterValues"]["toyType"] = toyType;

    std::string str(j.dump().c_str());
    return str;
}
std::string PickActionDescription::GetActionParametersJson_ForActionRegistration()
{
    json j;
    j["toyType"] = toyType;

    std::string str(j.dump().c_str());
    return str;
}    void NavigateActionDescription::SetActionParametersByState(std::tuple<int> parameters)
    {
        strLink_oDestination = "not applicable in this AOS version";
        oDestination = std::get<0>(parameters);
        
    }
void NavigateActionDescription::SetActionParametersByState(Collect_toysState *state, std::vector<std::string> indexes)
{
}
std::string NavigateActionDescription::GetActionParametersJson_ForActionExecution()
{  
    json j;
    j["ParameterLinks"]["oDestination"] = strLink_oDestination;
    j["ParameterValues"]["oDestination"] = oDestination;

    std::string str(j.dump().c_str());
    return str;
}
std::string NavigateActionDescription::GetActionParametersJson_ForActionRegistration()
{
    json j;
    j["oDestination"] = oDestination;

    std::string str(j.dump().c_str());
    return str;
}

void ActionManager::Init(Collect_toysState* state)
{
	
	int id = 0;
    ActionDescription *detect = new ActionDescription;
    detect->actionType = detectAction;
    detect->actionId = id++;
    ActionManager::actions.push_back(detect);

    ActionDescription *place = new ActionDescription;
    place->actionType = placeAction;
    place->actionId = id++;
    ActionManager::actions.push_back(place);

        vector<tuple<string>> pickParameters{};
        auto pickGetParameters = [&]()
        {
            vector<tuple<string>> __possibleParameters{};
            
            __possibleParameters.push_back(std::make_tuple("blue"));
            
__possibleParameters.push_back(std::make_tuple("red"));
            
__possibleParameters.push_back(std::make_tuple("black"));
            
__possibleParameters.push_back(std::make_tuple("green"));
            return __possibleParameters;
            
        };
        pickParameters = pickGetParameters();
        PickActionDescription* pickActions = new PickActionDescription[pickParameters.size()];
        for(int i=0;i< pickParameters.size();i++)
        {
            PickActionDescription &oPickAction = pickActions[i];
            oPickAction.SetActionParametersByState(pickParameters[i]);
            oPickAction.actionId = id++;
            oPickAction.actionType = pickAction;
            ActionManager::actions.push_back(&oPickAction);
        }
        vector<tuple<int>> navigateParameters{};
        auto navigateGetParameters = [&]()
        {
            vector<tuple<int>> __possibleParameters{};
            
            for(int i=0;
            i<=4;
            i++)
{
  __possibleParameters.push_back(std::make_tuple(i));
            
};
            return __possibleParameters;
            
        };
        navigateParameters = navigateGetParameters();
        NavigateActionDescription* navigateActions = new NavigateActionDescription[navigateParameters.size()];
        for(int i=0;i< navigateParameters.size();i++)
        {
            NavigateActionDescription &oNavigateAction = navigateActions[i];
            oNavigateAction.SetActionParametersByState(navigateParameters[i]);
            oNavigateAction.actionId = id++;
            oNavigateAction.actionType = navigateAction;
            ActionManager::actions.push_back(&oNavigateAction);
        }


    for(int j=0;j< ActionManager::actions.size();j++)
    {
        std::string actDesc = Prints::PrintActionDescription(ActionManager::actions[j]);
        MongoDB_Bridge::RegisterAction(ActionManager::actions[j]->actionId, enum_map_collect_toys::vecActionTypeEnumToString[ActionManager::actions[j]->actionType], ActionManager::actions[j]->GetActionParametersJson_ForActionRegistration(), actDesc);
    }
    
}
std::string Prints::PrintActionDescription(int actionId)
{
    return Prints::PrintActionDescription(ActionManager::actions[actionId]);
}


 
    std::string Prints::PrinteLocation(eLocation enumT)
    {
        switch (enumT)
        {
            case L0:
                return "L0";
            case L1:
                return "L1";
            case L2:
                return "L2";
            case L3:
                return "L3";
            case LChild:
                return "LChild";
            case LArm:
                return "LArm";
        }
        return "";

    }

    std::string Prints::PrintActionDescription(ActionDescription* act)
    {
        stringstream ss;
        ss << "ID:" << act->actionId;
        ss << "," << PrintActionType(act->actionType);
        if(act->actionType == pickAction)
        {
            PickActionDescription *pickA = static_cast<PickActionDescription *>(act);
            ss << "," << "toyType:" << pickA->toyType;
        }

        if(act->actionType == navigateAction)
        {
            NavigateActionDescription *navigateA = static_cast<NavigateActionDescription *>(act);
            ss << "," << "oDestination:" << navigateA->oDestination;
        }

        return ss.str();
    }



    std::string Prints::PrintState(Collect_toysState state)
    {
        stringstream ss;
        ss << "STATE: " << endl;
        ss << "|state.tGreen.location:";
        ss << state.tGreen.location;
        ss << "|state.tGreen.reward:";
        ss << state.tGreen.reward;
        ss << "|state.tGreen.type:";
        ss << state.tGreen.type;
        ss << "|state.observed_reward:";
        ss << state.observed_reward;
        ss << "|state.tBlue.location:";
        ss << state.tBlue.location;
        ss << "|state.tBlue.reward:";
        ss << state.tBlue.reward;
        ss << "|state.tBlue.type:";
        ss << state.tBlue.type;
        ss << "|state.tBlack.location:";
        ss << state.tBlack.location;
        ss << "|state.tBlack.reward:";
        ss << state.tBlack.reward;
        ss << "|state.tBlack.type:";
        ss << state.tBlack.type;
        ss << "|state.tRed.location:";
        ss << state.tRed.location;
        ss << "|state.tRed.reward:";
        ss << state.tRed.reward;
        ss << "|state.tRed.type:";
        ss << state.tRed.type;
        ss << "|state.pickActionsLeft:";
        ss << state.pickActionsLeft;
        ss << "|state.robotLocation:";
        ss << state.robotLocation;
        return ss.str();
    }


 
    std::string Prints::PrintActionType(ActionType actType)
    {
        switch (actType)
        {
        case detectAction:
            return "detectAction";
        case placeAction:
            return "placeAction";
        case pickAction:
            return "pickAction";
        case navigateAction:
            return "navigateAction";
        }
        return "";
    }

std::string Prints::GetStateJson(State& _state)
    {
        const Collect_toysState& state = static_cast<const Collect_toysState&>(_state);
        json j;
        j["tGreen"]["location"] = state.tGreen.location;
        j["tGreen"]["reward"] = state.tGreen.reward;
        j["tGreen"]["type"] = state.tGreen.type;
        j["observed_reward"] = state.observed_reward;
        j["tBlue"]["location"] = state.tBlue.location;
        j["tBlue"]["reward"] = state.tBlue.reward;
        j["tBlue"]["type"] = state.tBlue.type;
        j["tBlack"]["location"] = state.tBlack.location;
        j["tBlack"]["reward"] = state.tBlack.reward;
        j["tBlack"]["type"] = state.tBlack.type;
        j["tRed"]["location"] = state.tRed.location;
        j["tRed"]["reward"] = state.tRed.reward;
        j["tRed"]["type"] = state.tRed.type;
        j["pickActionsLeft"] = state.pickActionsLeft;
        j["robotLocation"] = state.robotLocation;

    std::string str(j.dump().c_str());
    return str;
     
    }

    void Prints::GetStateFromJson(Collect_toysState& state, std::string jsonStr, int stateIndex)
    {
        
        json j = json::parse(jsonStr);
        j = j["BeliefeState"];

        state.tGreen.location = j[stateIndex]["tGreen"]["location"];
        state.tGreen.reward = j[stateIndex]["tGreen"]["reward"];
        state.tGreen.type = j[stateIndex]["tGreen"]["type"];
        state.observed_reward = j[stateIndex]["observed_reward"];
        state.tBlue.location = j[stateIndex]["tBlue"]["location"];
        state.tBlue.reward = j[stateIndex]["tBlue"]["reward"];
        state.tBlue.type = j[stateIndex]["tBlue"]["type"];
        state.tBlack.location = j[stateIndex]["tBlack"]["location"];
        state.tBlack.reward = j[stateIndex]["tBlack"]["reward"];
        state.tBlack.type = j[stateIndex]["tBlack"]["type"];
        state.tRed.location = j[stateIndex]["tRed"]["location"];
        state.tRed.reward = j[stateIndex]["tRed"]["reward"];
        state.tRed.type = j[stateIndex]["tRed"]["type"];
        state.pickActionsLeft = j[stateIndex]["pickActionsLeft"];
        state.robotLocation = j[stateIndex]["robotLocation"];

    }



std::string Prints::PrintObs(int obs)
{
	Collect_toysResponseModuleAndTempEnums eObs = (Collect_toysResponseModuleAndTempEnums)obs;
	return enum_map_collect_toys::vecResponseEnumToString[eObs]; 
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
