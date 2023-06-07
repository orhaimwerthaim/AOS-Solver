
#include <string>
#include <vector>
#include <tuple>
#include <map>


#include <cstdint>
#include <iostream>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <random>
using namespace std;
namespace aos_model {
typedef bool anyValue;
typedef unsigned long int OBS_TYPE;
std::default_random_engine generator;
std::uniform_real_distribution<float> real_unfirom_dist(0.0,1.0); 
int SampleDiscrete(vector<float> weights)
{
    float rand = real_unfirom_dist(generator);
    float total = 0;
    for (int i = 0; i < weights.size();i++)
    {
        total += weights[i];
        if (rand < total)
            return i;
    }
    return -1;
}

bool SampleBernoulli(double p)
{
    float rand = real_unfirom_dist(generator); 
	
	return rand <= p;
}


  enum ActionType
{
    draw_in_cellAction,
    detect_board_stateAction
	
};



  enum IrosResponseModuleAndTempEnums
  {
	  draw_in_cell_moduleResponse,
	  draw_in_cell_res_success,
	  detect_board_state_moduleResponse,
	  cells,
	  c1,
	  c2,
	  c3,
	  c4,
	  c5,
	  c6,
	  c7,
	  c8,
	  c9,

      default_moduleResponse = 99999, 
	  illegalActionObs = 100000
  };

	  static map<IrosResponseModuleAndTempEnums,std::string> CreateMapResponseEnumToString()
	  {
          map<IrosResponseModuleAndTempEnums,std::string> m;
          m[draw_in_cell_res_success] = "draw_in_cell_res_success";
          m[c1] = "c1";
          m[c2] = "c2";
          m[c3] = "c3";
          m[c4] = "c4";
          m[c5] = "c5";
          m[c6] = "c6";
          m[c7] = "c7";
          m[c8] = "c8";
          m[c9] = "c9";
          m[illegalActionObs] = "IllegalActionObs";
          m[default_moduleResponse] = "DefaultObservation";
          return m;
        }

		static map<std::string, IrosResponseModuleAndTempEnums> CreateMapStringToEnum(map<IrosResponseModuleAndTempEnums,std::string> vecResponseEnumToString)
	  {
          map<std::string, IrosResponseModuleAndTempEnums> m;
		  map<IrosResponseModuleAndTempEnums,std::string>::iterator it;
		  for (it = vecResponseEnumToString.begin(); it != vecResponseEnumToString.end(); it++)
			{
				m[it->second] = it->first;
			}

          return m;
        }


		static map<ActionType,std::string> CreateMapActionTypeEnumToString()
	  {
          map<ActionType,std::string> m;
          m[draw_in_cellAction] = "draw_in_cell";
          m[detect_board_stateAction] = "detect_board_state";

          return m;
        }
 
	map<IrosResponseModuleAndTempEnums, std::string> vecResponseEnumToString;
	map<std::string, IrosResponseModuleAndTempEnums> vecStringToResponseEnum ;
	map<ActionType,std::string> vecActionTypeEnumToString;

	void InitMapEnumToString()
	{
		if(vecResponseEnumToString.size() > 0)
			return; 

		vecResponseEnumToString = CreateMapResponseEnumToString();
	    vecStringToResponseEnum = CreateMapStringToEnum(vecResponseEnumToString);
	    vecActionTypeEnumToString = CreateMapActionTypeEnumToString();
	}



	enum tCellContent
	{
		eX,
		eO,
		eEmpty
	};




	struct tGridCell
	{
		int location;
		tCellContent content;
        void set_content( int c_) { content = (tCellContent)c_; }
        int get_content()  { return (int)content; }
		inline bool operator==(const tGridCell& other)const{return (*this).location == other.location && (*this).content == other.content;};
		inline bool operator!=(const tGridCell& other)const{return !(*this == other);};
		tGridCell(); 
	};


class State {
public:
	int state_id;

	State();
	State(int _state_id);
	virtual ~State();

	friend std::ostream& operator<<(std::ostream& os, const State& state);
 

	static double Weight(const std::vector<State*>& particles);

	State* operator()(int state_id) {
		this->state_id = state_id; 
		return this;
	}

    bool __isTermianl = false;
    bool OneTimeRewardUsed[3]={true,true,true};
    std::vector<tCellContent> tCellContentObjects;
    std::vector<tGridCell*> tGridCellObjects;
    std::map<std::string, tCellContent> tCellContentObjectsForActions;
    std::map<std::string, int> intObjectsForActions;
    tCellContent robotSymbol;
    tCellContent humanSymbol;
    int CellLocation1;
    int CellLocation2;
    int CellLocation3;
    int CellLocation4;
    int CellLocation5;
    int CellLocation6;
    int CellLocation7;
    int CellLocation8;
    int CellLocation9;
    tGridCell Cell1;
    tGridCell Cell2;
    tGridCell Cell3;
    tGridCell Cell4;
    tGridCell Cell5;
    tGridCell Cell6;
    tGridCell Cell7;
    tGridCell Cell8;
    tGridCell Cell9;
    bool isRobotTurn;
            void set_robotSymbol( int c_) { robotSymbol = (tCellContent)c_; }
        int get_robotSymbol()  { return (int)robotSymbol; }

            void set_humanSymbol( int c_) { humanSymbol = (tCellContent)c_; }
        int get_humanSymbol()  { return (int)humanSymbol; }

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    std::map<std::string, anyValue*> anyValueUpdateDic;


	public:
		static void SetAnyValueLinks(State *state);
};
typedef State IrosState;

State::State(){}
State::~State() {}

State* CopyToNewState(State* state)
{
    State* s= new State();
    *s=*state;
    return s;
}

State* CopyToState(State* state, State* toState)
{
    *toState=*state;
    return toState;
}

State* prevState = new State();
State* afterExtState = new State();




    class ActionDescription
    {
    public:
        int actionId;
        ActionType actionType;
        virtual void SetActionParametersByState(State *state, std::vector<std::string> indexes);
        virtual std::string GetActionParametersJson_ForActionExecution() { return ""; };
        virtual std::string GetActionParametersJson_ForActionRegistration() { return ""; };
        
    };

class Draw_in_cellActionDescription: public ActionDescription
{
    public:
        int oCellP;
        tCellContent symbol;
        std::string strLink_oCellP;
        std::string strLink_symbol;
        Draw_in_cellActionDescription(int _oCellP_Index, int _symbol_Index);
        virtual void SetActionParametersByState(State *state, std::vector<std::string> indexes);
Draw_in_cellActionDescription(){};
};

class Detect_board_stateActionDescription: public ActionDescription
{
    public:
        //Detect_board_stateActionDescription();
        virtual void SetActionParametersByState(State *state, std::vector<std::string> indexes);
Detect_board_stateActionDescription(){};
};



class ActionManager {
public:
	static std::vector<ActionDescription*> actions;
    static void Init(State* state);
};


class Prints
{
	public:
    static std::string PrinttCellContent(tCellContent);

	static std::string PrintActionDescription(ActionDescription*);
	static std::string PrintActionType(ActionType);
	static std::string PrintState(State state);
	static std::string PrintObs(int action, int obs);
    static void SaveBeliefParticles(vector<State *> particles);
    static std::string GetStateJson(State &state);
    static void GetStateFromJson(State &state, std::string jsonStr, int stateIndex);
};



    void ActionDescription::SetActionParametersByState(IrosState *state, std::vector<std::string> indexes){}
    std::vector<ActionDescription*> ActionManager::actions;


void Draw_in_cellActionDescription::SetActionParametersByState(IrosState *state, std::vector<std::string> indexes)
{
    strLink_oCellP = indexes[0];
    oCellP = (state->intObjectsForActions[indexes[0]]);
    strLink_symbol = indexes[1];
    symbol = (state->tCellContentObjectsForActions[indexes[1]]);
}


void ActionManager::Init(IrosState* state)
{
	
	int id = 0;
    Draw_in_cellActionDescription* draw_in_cellActions = new Draw_in_cellActionDescription[9];
    std::vector<std::string> draw_in_cellIndexes;
    int draw_in_cellActCounter = 0;
    map<std::string, int>::iterator draw_in_cellIt1;
    for (draw_in_cellIt1 = state->intObjectsForActions.begin(); draw_in_cellIt1 != state->intObjectsForActions.end(); draw_in_cellIt1++)
    {
        draw_in_cellIndexes.push_back(draw_in_cellIt1->first);
        map<std::string, tCellContent>::iterator draw_in_cellIt2;
        for (draw_in_cellIt2 = state->tCellContentObjectsForActions.begin(); draw_in_cellIt2 != state->tCellContentObjectsForActions.end(); draw_in_cellIt2++)
        {
            draw_in_cellIndexes.push_back(draw_in_cellIt2->first);
            Draw_in_cellActionDescription &oDraw_in_cellAction = draw_in_cellActions[draw_in_cellActCounter];
            oDraw_in_cellAction.SetActionParametersByState(state, draw_in_cellIndexes);
            oDraw_in_cellAction.actionId = id++;
            oDraw_in_cellAction.actionType = draw_in_cellAction;
            ActionManager::actions.push_back(&oDraw_in_cellAction);
            draw_in_cellActCounter++;
            draw_in_cellIndexes.pop_back();
        }
            draw_in_cellIndexes.pop_back();
    }
    ActionDescription *detect_board_state = new ActionDescription;
    detect_board_state->actionType = detect_board_stateAction;
    detect_board_state->actionId = id++;
    ActionManager::actions.push_back(detect_board_state);


}


 
    std::string Prints::PrinttCellContent(tCellContent enumT)
    {
        switch (enumT)
        {
            case eX:
                return "eX";
            case eO:
                return "eO";
            case eEmpty:
                return "eEmpty";
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
            ss << "," << "symbol:" << Prints::PrinttCellContent((tCellContent)draw_in_cellA->symbol);;
        }

        return ss.str();
    }



    std::string Prints::PrintState(IrosState state)
    {
        stringstream ss;
        ss << "STATE: ";
        ss << "|state.humanSymbol:";
        ss << state.humanSymbol;
        ss << "|state.Cell1.location:";
        ss << state.Cell1.location;
        ss << "|state.Cell1.content:";
        ss << state.Cell1.content;
        ss << "|state.Cell2.location:";
        ss << state.Cell2.location;
        ss << "|state.Cell2.content:";
        ss << state.Cell2.content;
        ss << "|state.Cell3.location:";
        ss << state.Cell3.location;
        ss << "|state.Cell3.content:";
        ss << state.Cell3.content;
        ss << "|state.Cell4.location:";
        ss << state.Cell4.location;
        ss << "|state.Cell4.content:";
        ss << state.Cell4.content;
        ss << "|state.Cell5.location:";
        ss << state.Cell5.location;
        ss << "|state.Cell5.content:";
        ss << state.Cell5.content;
        ss << "|state.Cell6.location:";
        ss << state.Cell6.location;
        ss << "|state.Cell6.content:";
        ss << state.Cell6.content;
        ss << "|state.Cell7.location:";
        ss << state.Cell7.location;
        ss << "|state.Cell7.content:";
        ss << state.Cell7.content;
        ss << "|state.Cell8.location:";
        ss << state.Cell8.location;
        ss << "|state.Cell8.content:";
        ss << state.Cell8.content;
        ss << "|state.Cell9.location:";
        ss << state.Cell9.location;
        ss << "|state.Cell9.content:";
        ss << state.Cell9.content;
        ss << "|state.isRobotTurn:";
        ss << state.isRobotTurn;
        return ss.str();
    }


 
    std::string Prints::PrintActionType(ActionType actType)
    {
        switch (actType)
        {
        case draw_in_cellAction:
            return "draw_in_cellAction";
        case detect_board_stateAction:
            return "detect_board_stateAction";
        }
        return "";
    }

    tGridCell::tGridCell()
    {
        location = -1;
        content = eEmpty;
    }





		void IrosState::SetAnyValueLinks(IrosState *state)
		{

		}

bool ProcessSpecialStates(State &state, double &reward)
{
    float temp_reward = 0;
    bool temp_IsGoalState = false;
    bool temp_StopEvaluatingState = false;
    bool isFinalState = false;
    if(state.OneTimeRewardUsed[0])
    {
        if ((state.robotSymbol == state.Cell9.content && state.Cell9.content == state.Cell8.content && state.Cell8.content == state.Cell7.content) || (state.robotSymbol == state.Cell3.content && state.Cell3.content == state.Cell2.content && state.Cell2.content == state.Cell1.content) || (state.robotSymbol == state.Cell6.content && state.Cell6.content == state.Cell5.content && state.Cell5.content == state.Cell4.content) || (state.robotSymbol == state.Cell7.content && state.Cell7.content == state.Cell4.content && state.Cell4.content == state.Cell1.content) || (state.robotSymbol == state.Cell8.content && state.Cell8.content == state.Cell5.content && state.Cell5.content == state.Cell2.content)|| (state.robotSymbol == state.Cell9.content && state.Cell9.content == state.Cell6.content && state.Cell6.content == state.Cell3.content) || (state.robotSymbol == state.Cell9.content && state.Cell9.content == state.Cell5.content && state.Cell5.content == state.Cell1.content) || (state.robotSymbol == state.Cell7.content && state.Cell7.content == state.Cell5.content && state.Cell5.content == state.Cell3.content))
        {
            reward += 10;
            isFinalState = true;
        }
    }
    if(state.OneTimeRewardUsed[1])
    {
        if ((state.humanSymbol == state.Cell9.content && state.Cell9.content == state.Cell8.content && state.Cell8.content == state.Cell7.content) || (state.humanSymbol == state.Cell3.content && state.Cell3.content == state.Cell2.content && state.Cell2.content == state.Cell1.content) || (state.humanSymbol == state.Cell6.content && state.Cell6.content == state.Cell5.content && state.Cell5.content == state.Cell4.content) || (state.humanSymbol == state.Cell7.content && state.Cell7.content == state.Cell4.content && state.Cell4.content == state.Cell1.content) || (state.humanSymbol == state.Cell8.content && state.Cell8.content == state.Cell5.content && state.Cell5.content == state.Cell2.content)|| (state.humanSymbol == state.Cell9.content && state.Cell9.content == state.Cell6.content && state.Cell6.content == state.Cell3.content) || (state.humanSymbol == state.Cell9.content && state.Cell9.content == state.Cell5.content && state.Cell5.content == state.Cell1.content) || (state.humanSymbol == state.Cell7.content && state.Cell7.content == state.Cell5.content && state.Cell5.content == state.Cell3.content))
        {
            reward += -10;
            isFinalState = true;
        }
    }
    if(state.OneTimeRewardUsed[2])
    {
        if (state.Cell1.content != eEmpty && state.Cell2.content != eEmpty && state.Cell3.content != eEmpty && state.Cell4.content != eEmpty && state.Cell5.content != eEmpty && state.Cell6.content != eEmpty && state.Cell7.content != eEmpty && state.Cell8.content != eEmpty && state.Cell9.content != eEmpty)
        {
            reward += 0;
            isFinalState = true;
        }
    }
    return isFinalState;
}
State* CreateStartState() {
    State* startState = new State();
    IrosState& state = *startState;
    startState->tCellContentObjects.push_back(eX);
    startState->tCellContentObjects.push_back(eO);
    startState->tCellContentObjects.push_back(eEmpty);
    state.robotSymbol=eO;
    state.humanSymbol=eX;
    state.CellLocation1=1;
    state.CellLocation2=2;
    state.CellLocation3=3;
    state.CellLocation4=4;
    state.CellLocation5=5;
    state.CellLocation6=6;
    state.CellLocation7=7;
    state.CellLocation8=8;
    state.CellLocation9=9;
    state.Cell1 = tGridCell();
    state.Cell1.content=eEmpty;
    state.Cell1.location=1;
    state.Cell2 = tGridCell();
    state.Cell2.content=eEmpty;
    state.Cell2.location=2;
    state.Cell3 = tGridCell();
    state.Cell3.content=eEmpty;
    state.Cell3.location=3;
    state.Cell4 = tGridCell();
    state.Cell4.content=eEmpty;
    state.Cell4.location=4;
    state.Cell5 = tGridCell();
    state.Cell5.content=eEmpty;
    state.Cell5.location=5;
    state.Cell6 = tGridCell();
    state.Cell6.content=eEmpty;
    state.Cell6.location=6;
    state.Cell7 = tGridCell();
    state.Cell7.content=eEmpty;
    state.Cell7.location=7;
    state.Cell8 = tGridCell();
    state.Cell8.content=eEmpty;
    state.Cell8.location=8;
    state.Cell9 = tGridCell();
    state.Cell9.content=eEmpty;
    state.Cell9.location=9;
    state.isRobotTurn=false;
    startState->tCellContentObjectsForActions["state.robotSymbol"] = (state.robotSymbol);
    startState->intObjectsForActions["state.CellLocation1"] = (state.CellLocation1);
    startState->intObjectsForActions["state.CellLocation2"] = (state.CellLocation2);
    startState->intObjectsForActions["state.CellLocation3"] = (state.CellLocation3);
    startState->intObjectsForActions["state.CellLocation4"] = (state.CellLocation4);
    startState->intObjectsForActions["state.CellLocation5"] = (state.CellLocation5);
    startState->intObjectsForActions["state.CellLocation6"] = (state.CellLocation6);
    startState->intObjectsForActions["state.CellLocation7"] = (state.CellLocation7);
    startState->intObjectsForActions["state.CellLocation8"] = (state.CellLocation8);
    startState->intObjectsForActions["state.CellLocation9"] = (state.CellLocation9);
    startState->tGridCellObjects.push_back(&(state.Cell1));
    startState->tGridCellObjects.push_back(&(state.Cell2));
    startState->tGridCellObjects.push_back(&(state.Cell3));
    startState->tGridCellObjects.push_back(&(state.Cell4));
    startState->tGridCellObjects.push_back(&(state.Cell5));
    startState->tGridCellObjects.push_back(&(state.Cell6));
    startState->tGridCellObjects.push_back(&(state.Cell7));
    startState->tGridCellObjects.push_back(&(state.Cell8));
    startState->tGridCellObjects.push_back(&(state.Cell9));
    if (ActionManager::actions.size() == 0)
    {
        ActionManager::Init(const_cast <IrosState*> (startState));
    }
    double r;
    state.__isTermianl = ProcessSpecialStates(state, r);
    return startState;
}

void CheckPreconditions(const State& state, double &reward, bool &__meetPrecondition, int actionId)
    {
        ActionType &actType = ActionManager::actions[actionId]->actionType;
        __meetPrecondition = true;
            if(actType == draw_in_cellAction)
            {
                Draw_in_cellActionDescription act = *(static_cast<Draw_in_cellActionDescription *>(ActionManager::actions[actionId]));
                int &oCellP = act.oCellP;
                tCellContent &symbol = act.symbol;
                __meetPrecondition=state.isRobotTurn;
                for (int ind0 = 0; ind0 < state.tGridCellObjects.size(); ind0++)
                {
                    tGridCell cell = *(state.tGridCellObjects[ind0]);
                    if (cell.location== oCellP && cell.content != eEmpty)
                    {
                        __meetPrecondition= false;
                    }
                }
                if(!__meetPrecondition) reward += -10;
            }
            if(actType == detect_board_stateAction)
            {
                __meetPrecondition=!state.isRobotTurn;
                if(!__meetPrecondition) reward += -10;
            }
    }

void ComputePreferredActionValue(const State& state, double &__heuristicValue, int actionId)
    {
        __heuristicValue = 0;
        ActionType &actType = ActionManager::actions[actionId]->actionType;
            if(actType == draw_in_cellAction)
            {
                Draw_in_cellActionDescription act = *(static_cast<Draw_in_cellActionDescription *>(ActionManager::actions[actionId]));
                int &oCellP = act.oCellP;
                tCellContent &symbol = act.symbol;
            }
            if(actType == detect_board_stateAction)
            {
            }
        __heuristicValue = __heuristicValue < 0 ? 0 : __heuristicValue;
    }

void SampleModuleExecutionTime(const State& farstate, int actionId, int &__moduleExecutionTime)
{
    ActionType &actType = ActionManager::actions[actionId]->actionType;
    if(actType == draw_in_cellAction)
    {
    }
    if(actType == detect_board_stateAction)
    {
    }
}

void ExtrinsicChangesDynamicModel(const State& state, State& state_, int actionId, const int &__moduleExecutionTime,  double &__reward)
{
    ActionType &actionType = ActionManager::actions[actionId]->actionType;
    Draw_in_cellActionDescription* draw_in_cell = actionType != (draw_in_cellAction) ? NULL : (static_cast<Draw_in_cellActionDescription *>(ActionManager::actions[actionId]));
    Detect_board_stateActionDescription* detect_board_state = actionType != (detect_board_stateAction) ? NULL : (static_cast<Detect_board_stateActionDescription *>(ActionManager::actions[actionId]));
    int sampledCell;
    IrosResponseModuleAndTempEnums  __S;
    while(!state.isRobotTurn)
{
   vector<float> weights{0.1111,0.1111,0.1111,0.1111,0.1111,0.1111,0.1111,0.1111,0.1112};
    
   sampledCell = cells + SampleDiscrete(weights);
    
   //sampledCell = (IrosResponseModuleAndTempEnums)(cells + 1 + Iros::Environment_discrete_dist1(Iros::generator));
    
   if(sampledCell==c1 && state.Cell1.content==eEmpty){state_.Cell1.content=eX;
    break;
    }
   if(sampledCell==c2 && state.Cell2.content==eEmpty){state_.Cell2.content=eX;
    break;
    }
   if(sampledCell==c3 && state.Cell3.content==eEmpty){state_.Cell3.content=eX;
    break;
    }
   if(sampledCell==c4 && state.Cell4.content==eEmpty){state_.Cell4.content=eX;
    break;
    }
   if(sampledCell==c5 && state.Cell5.content==eEmpty){state_.Cell5.content=eX;
    break;
    }
   if(sampledCell==c6 && state.Cell6.content==eEmpty){state_.Cell6.content=eX;
    break;
    }
   if(sampledCell==c7 && state.Cell7.content==eEmpty){state_.Cell7.content=eX;
    break;
    }
   if(sampledCell==c8 && state.Cell8.content==eEmpty){state_.Cell8.content=eX;
    break;
    }
   if(sampledCell==c9 && state.Cell9.content==eEmpty){state_.Cell9.content=eX;
    break;
    }
};
}

void ModuleDynamicModel(const State &state, const State &state_, State &state__, int actionId, double &__reward, OBS_TYPE &observation, const int &__moduleExecutionTime, const bool &__meetPrecondition)
{
    std::hash<std::string> hasher;
    ActionType &actType = ActionManager::actions[actionId]->actionType;
    observation = -1;
    observation = default_moduleResponse;
    std::string __moduleResponseStr = "NoStrResponse";
    OBS_TYPE &__moduleResponse = observation;
    if(actType == draw_in_cellAction)
    {
        Draw_in_cellActionDescription act = *(static_cast<Draw_in_cellActionDescription *>(ActionManager::actions[actionId]));
        int &oCellP = act.oCellP;
        tCellContent &symbol = act.symbol;
        for (int ind0 = 0; ind0 < state.tGridCellObjects.size(); ind0++)
        {
            tGridCell &cell = *(state__.tGridCellObjects[ind0]);
            if (cell.location== oCellP && cell.content == eEmpty)
            {
                cell.content=symbol;
                break;
            }
        }
        state__.isRobotTurn=!state.isRobotTurn;
        __moduleResponse= draw_in_cell_res_success;
        __reward = 0;
    }
    if(actType == detect_board_stateAction)
    {
        state__.isRobotTurn=!state.isRobotTurn;
        __moduleResponseStr = "_________";
        __moduleResponseStr[0] = state__.Cell1.content== eX ? 'X' : (state__.Cell1.content== eO ? 'O' : '?');
        __moduleResponseStr[1] = state__.Cell2.content== eX ? 'X' : (state__.Cell2.content== eO ? 'O' : '?');
        __moduleResponseStr[2] = state__.Cell3.content== eX ? 'X' : (state__.Cell3.content== eO ? 'O' : '?');
        __moduleResponseStr[3] = state__.Cell4.content== eX ? 'X' : (state__.Cell4.content== eO ? 'O' : '?');
        __moduleResponseStr[4] = state__.Cell5.content== eX ? 'X' : (state__.Cell5.content== eO ? 'O' : '?');
        __moduleResponseStr[5] = state__.Cell6.content== eX ? 'X' : (state__.Cell6.content== eO ? 'O' : '?');
        __moduleResponseStr[6] = state__.Cell7.content== eX ? 'X' : (state__.Cell7.content== eO ? 'O' : '?');
        __moduleResponseStr[7] = state__.Cell8.content== eX ? 'X' : (state__.Cell8.content== eO ? 'O' : '?');
        __moduleResponseStr[8] = state__.Cell9.content== eX ? 'X' : (state__.Cell9.content== eO ? 'O' : '?');
        __reward = 0;
    }
    if(__moduleResponseStr != "NoStrResponse")
    {
        IrosResponseModuleAndTempEnums responseHash = (IrosResponseModuleAndTempEnums)hasher(__moduleResponseStr);
        vecResponseEnumToString[responseHash] = __moduleResponseStr;
        vecStringToResponseEnum[__moduleResponseStr] = responseHash;
        __moduleResponse = responseHash;
    }
}

bool Step(State& state__, int actionId, double& reward,
	OBS_TYPE& observation) {    observation = default_moduleResponse;    reward=0; 
	int __moduleExecutionTime = -1;
	bool meetPrecondition = false;
	double tReward = 0;
 
	CheckPreconditions(state__, reward, meetPrecondition, actionId);
	State *p_state = CopyToState(&state__, afterExtState);//Copy(&s_state__);
        State state = *p_state;
	//IrosState &state = static_cast<IrosState &>(*s_state);

	
	SampleModuleExecutionTime(state__, rand_num, actionId, __moduleExecutionTime);

	ExtrinsicChangesDynamicModel(state, state__, actionId, __moduleExecutionTime, tReward);
    reward += tReward;
    tReward = 0;
    

	//State *s_state_ = Copy(&s_state__);
    State *p_state_ = CopyToState(&state__, afterExtState);
    State state_=*p_state_;
	//IrosState &state_ = static_cast<IrosState &>(*s_state_);

    
	ModuleDynamicModel(state, state_, state__, actionId, tReward,
					   observation, __moduleExecutionTime, meetPrecondition);
    
	
    //delete s_state_;
    //delete s_state;
	reward += tReward;
	bool finalState = ProcessSpecialStates(state__, reward);
    state__.__isTermianl = state__.__isTermianl || finalState;

return finalState; 
}



    State* InitEnv()
    {
        InitMapEnumToString();
        State* state = CreateStartState();
        return state;
    }
 
}