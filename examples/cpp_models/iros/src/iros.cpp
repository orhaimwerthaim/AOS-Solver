#include "iros.h"
#include <despot/core/pomdp.h> 
#include <stdlib.h>
#include <despot/solver/pomcp.h>
#include <sstream>
#include <despot/model_primitives/iros/actionManager.h> 
#include <despot/model_primitives/iros/enum_map_iros.h>  
#include <algorithm>
#include <cmath> 
#include <despot/util/mongoDB_Bridge.h>
#include <functional> //for std::hash
#include <set>
#include <unistd.h>
#include <iomanip>
#include <float.h>
#include <map>
#include <tuple>

using namespace std;

namespace despot {

Iros Iros::gen_model;

std::uniform_real_distribution<float> AOSUtils::real_unfirom_dist(0.0,1.0);
std::default_random_engine AOSUtils::generator(std::random_device{}());

int AOSUtils::SampleDiscrete(vector<float> weights)
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

int AOSUtils::SampleDiscrete(vector<double> weights)
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

bool AOSUtils::Bernoulli(double p)
{
    float rand = real_unfirom_dist(generator);
	return rand < p;
}
std::hash<std::string> Iros::hasher;
/* ==============================================================================
 *IrosBelief class
 * ==============================================================================*/
int IrosBelief::num_particles = 5234;
std::string IrosBelief::beliefFromDB = "";
int IrosBelief::currentInitParticleIndex = -1;

IrosBelief::IrosBelief(vector<State*> particles, const DSPOMDP* model,
	Belief* prior) :
	ParticleBelief(particles, model, prior),
	iros_(static_cast<const Iros*>(model)) {
}

	
 	std::string Iros::GetActionDescription(int actionId) const
	 {
		 return Prints::PrintActionDescription(ActionManager::actions[actionId]);
	 }

void IrosBelief::Update(int actionId, OBS_TYPE obs, map<std::string, std::string> localVariables) {
	history_.Add(actionId, obs);

    ActionType &actType = ActionManager::actions[actionId]->actionType;
    string obsr;
    int cell_to_mark;
    bool success;



    try
    {
        if(actType == draw_in_cellAction)
        {
            if(localVariables.find("cell_to_mark") != localVariables.end())
            {
                cell_to_mark = std::stoi(localVariables["cell_to_mark"]);
            }
            if(localVariables.find("success") != localVariables.end())
            {
                success = localVariables["success"] == "true";
            }
        }
        if(actType == detect_board_stateAction)
        {
            if(localVariables.find("obsr") != localVariables.end())
            {
                obsr = localVariables["obsr"];
            }
        }

    }
    catch(const std::exception& e)
    {
        std::string s ="Error: problem loading LocalVariables data for belief state update. ";
        MongoDB_Bridge::AddError(s + e.what());
    }

	vector<State*> updated;
	double reward;
	OBS_TYPE o;
	int cur = 0, N = particles_.size(), trials = 0;
	while (updated.size() < num_particles && trials < 10 * num_particles) {
		State* particle = iros_->Copy(particles_[cur]);
		bool terminal = iros_->Step(*particle, Random::RANDOM.NextDouble(),
			actionId, reward, o);
 
		//if (!terminal && o == obs)
        if (o == obs) 
			{
                if(!Globals::IsInternalSimulation())
                {
				IrosState &state__ = static_cast<IrosState &>(*particles_[cur]);
                IrosState &state___ = static_cast<IrosState &>(*particle);
                    if(actType == draw_in_cellAction)
                    {
                        Draw_in_cellActionDescription act = *(static_cast<Draw_in_cellActionDescription *>(ActionManager::actions[actionId]));
                        int &oCellP = act.oCellP;
                    }
                    if(actType == detect_board_stateAction)
                    {
                    }



				//if(!Globals::IsInternalSimulation() && updates.size() > 0)
				//{
				//	IrosState::SetAnyValueLinks(&iros_particle);
				//	map<std::string, bool>::iterator it;
				//	for (it = updates.begin(); it != updates.end(); it++)
				//	{
				//		*(iros_particle.anyValueUpdateDic[it->first]) = it->second; 
				//	} 
				//}

                }
				updated.push_back(particle);
		} else {
			iros_->Free(particle);
		}

		cur = (cur + 1) % N;

		trials++;
	}

	for (int i = 0; i < particles_.size(); i++)
		iros_->Free(particles_[i]);

	particles_ = updated;

	for (int i = 0; i < particles_.size(); i++)
		particles_[i]->weight = 1.0 / particles_.size();
 
}

/* ==============================================================================
 * IrosPOMCPPrior class
 * ==============================================================================*/

class IrosPOMCPPrior: public POMCPPrior {
private:
	const Iros* iros_;

public:
	IrosPOMCPPrior(const Iros* model) :
		POMCPPrior(model),
		iros_(model) {
	}

	void ComputePreference(const State& state) {
		const IrosState& iros_state = static_cast<const IrosState&>(state);
		weighted_preferred_actions_.clear();
        legal_actions_.clear();
		preferred_actions_.clear();
        std::vector<double> weighted_preferred_actions_un_normalized;

        double heuristicValueTotal = 0;
		for (int a = 0; a < 1; a++) {
            weighted_preferred_actions_un_normalized.push_back(0);
			double reward = 0;
			bool meetPrecondition = false; 
			Iros::CheckPreconditions(iros_state, reward, meetPrecondition, a);
            if(meetPrecondition)
            {
                legal_actions_.push_back(a);
                double __heuristicValue; 
                Iros::ComputePreferredActionValue(iros_state, __heuristicValue, a);
                heuristicValueTotal += __heuristicValue;
                weighted_preferred_actions_un_normalized[a]=__heuristicValue;
            }
        }

        if(heuristicValueTotal > 0)
        {
            for (int a = 0; a < 1; a++) 
            {
                weighted_preferred_actions_.push_back(weighted_preferred_actions_un_normalized[a] / heuristicValueTotal);
            } 
        }
    }
};

/* ==============================================================================
 * Iros class
 * ==============================================================================*/

Iros::Iros(){
	srand((unsigned int)time(NULL));
}

int Iros::NumActions() const {
	return ActionManager::actions.size();
}

double Iros::ObsProb(OBS_TYPE obs, const State& state, int actionId) const {
	return 0.9;
}

 


std::default_random_engine Iros::generator;


State* Iros::CreateStartState(string type) const {
    IrosState* startState = memory_pool_.Allocate();
    IrosState& state = *startState;
    startState->tSymbolsObjects.push_back(eEmpty);
    startState->tSymbolsObjects.push_back(eO);
    startState->tSymbolsObjects.push_back(eX);
    state.grid={eEmpty,eEmpty,eEmpty,eEmpty,eEmpty,eEmpty,eEmpty,eEmpty,eEmpty};
    state.isRobotTurn=true;
    if (ActionManager::actions.size() == 0)
    {
        ActionManager::Init(const_cast <IrosState*> (startState));
    }
    double r;
    state.__isTermianl = ProcessSpecialStates(state, r);
    return startState;
}




Belief* Iros::InitialBelief(const State* start, string type) const {
    if(Globals::config.solveProblemWithClosedPomdpModel)
    {
        POMDP_ClosedModel::closedModel.CreateAndSolveModel();
    }
	int N = IrosBelief::num_particles;
	vector<State*> particles(N);
	for (int i = 0; i < N; i++) {
        IrosBelief::currentInitParticleIndex = i;
		particles[i] = CreateStartState();
		particles[i]->weight = 1.0 / N;
	}
    IrosBelief::currentInitParticleIndex = -1;
	return new IrosBelief(particles, this);
}
 

 

POMCPPrior* Iros::CreatePOMCPPrior(string name) const { 
		return new IrosPOMCPPrior(this);
}

void Iros::PrintState(const State& state, ostream& ostr) const {
	const IrosState& farstate = static_cast<const IrosState&>(state);
	if (ostr)
		ostr << Prints::PrintState(farstate);
}

void Iros::PrintObs(const State& state, OBS_TYPE observation,
	ostream& ostr) const {
	const IrosState& farstate = static_cast<const IrosState&>(state);
	
	ostr << observation <<endl;
}

void Iros::PrintBelief(const Belief& belief, ostream& out) const {
	 out << "called PrintBelief(): b printed"<<endl;
		out << endl;
	
}

void Iros::PrintAction(int actionId, ostream& out) const {
	out << Prints::PrintActionDescription(ActionManager::actions[actionId]) << endl;
}

State* Iros::Allocate(int state_id, double weight) const {
	IrosState* state = memory_pool_.Allocate();
	state->state_id = state_id;
	state->weight = weight;
	return state;
}

State* Iros::Copy(const State* particle) const {
	IrosState* state = memory_pool_.Allocate();
	*state = *static_cast<const IrosState*>(particle);
	state->SetAllocated();



	return state;
}

void Iros::Free(State* particle) const {
	memory_pool_.Free(static_cast<IrosState*>(particle));
}

int Iros::NumActiveParticles() const {
	return memory_pool_.num_allocated();
}

void Iros::StepForModel(State& state, int actionId, double& reward,
        OBS_TYPE& observation, int &state_hash, int &next_state_hash, bool& isTerminal, double& precondition_reward, double& specialStateReward) const
    {
        reward = 0;
        IrosState &ir_state = static_cast<IrosState &>(state);
        state_hash = hasher(Prints::PrintState(ir_state));

        bool meetPrecondition;
        precondition_reward = 0;
        CheckPreconditions(ir_state, reward, meetPrecondition, actionId);
        if(!meetPrecondition)
        {
            precondition_reward = reward;
        }
        
        isTerminal = Iros::Step(state, 0.1, actionId, reward,
                   observation);
        ir_state = static_cast<IrosState &>(state);

        specialStateReward = 0;
        ProcessSpecialStates(ir_state, specialStateReward);
        reward -= (precondition_reward + specialStateReward);//so that it will not consider the precondition penalty and special states reward

        next_state_hash = hasher(Prints::PrintState(ir_state));
    }
bool Iros::Step(State& s_state__, double rand_num, int actionId, double& reward,
	OBS_TYPE& observation) const {
    observation = default_moduleResponse;
    reward = 0;
	Random random(rand_num);
	int __moduleExecutionTime = -1;
	bool meetPrecondition = false;
	double tReward = 0;

	IrosState &state__ = static_cast<IrosState &>(s_state__);
	 logd << "[Iros::Step] Selected Action:" << Prints::PrintActionDescription(ActionManager::actions[actionId]) << "||State"<< Prints::PrintState(state__);
	CheckPreconditions(state__, reward, meetPrecondition, actionId);
	State *s_state = Copy(&s_state__);
	IrosState &state = static_cast<IrosState &>(*s_state);

	
	SampleModuleExecutionTime(state__, rand_num, actionId, __moduleExecutionTime);

	ExtrinsicChangesDynamicModel(state, state__, rand_num, actionId, __moduleExecutionTime, tReward);
    reward += tReward;
    tReward = 0;
    

	State *s_state_ = Copy(&s_state__);
	IrosState &state_ = static_cast<IrosState &>(*s_state_);

    
	ModuleDynamicModel(state, state_, state__, rand_num, actionId, tReward,
					   observation, __moduleExecutionTime, meetPrecondition);
    
	
    Free(s_state_);
    Free(s_state);
	reward += tReward;
	bool finalState = ProcessSpecialStates(state__, reward);
    state__.__isTermianl = state__.__isTermianl || finalState;

    if (!meetPrecondition)
	{
		//__moduleExecutionTime = 0;
		//observation = illegalActionObs;
		//return false;
	}
	return finalState;
}

void Iros::CheckPreconditions(const IrosState& state, double &reward, bool &__meetPrecondition, int actionId)
    {
        ActionType &actType = ActionManager::actions[actionId]->actionType;
        __meetPrecondition = true;
            if(actType == draw_in_cellAction)
            {
                Draw_in_cellActionDescription act = *(static_cast<Draw_in_cellActionDescription *>(ActionManager::actions[actionId]));
                int &oCellP = act.oCellP;
                __meetPrecondition=state.isRobotTurn && state.grid[oCellP] == eEmpty;
                if(!__meetPrecondition) reward += -10;
            }
            if(actType == detect_board_stateAction)
            {
                __meetPrecondition=!state.isRobotTurn;
                if(!__meetPrecondition) reward += -10;
            }
    }

void Iros::ComputePreferredActionValue(const IrosState& state, double &__heuristicValue, int actionId)
    {
        __heuristicValue = 0;
        ActionType &actType = ActionManager::actions[actionId]->actionType;
            if(actType == draw_in_cellAction)
            {
                Draw_in_cellActionDescription act = *(static_cast<Draw_in_cellActionDescription *>(ActionManager::actions[actionId]));
                int &oCellP = act.oCellP;
            }
            if(actType == detect_board_stateAction)
            {
            }
        __heuristicValue = __heuristicValue < 0 ? 0 : __heuristicValue;
    }

void Iros::SampleModuleExecutionTime(const IrosState& farstate, double rand_num, int actionId, int &__moduleExecutionTime) const
{
    ActionType &actType = ActionManager::actions[actionId]->actionType;
    if(actType == draw_in_cellAction)
    {
    }
    if(actType == detect_board_stateAction)
    {
    }
}

void Iros::ExtrinsicChangesDynamicModel(const IrosState& state, IrosState& state_, double rand_num, int actionId, const int &__moduleExecutionTime,  double &__reward)  const
{
    ActionType &actionType = ActionManager::actions[actionId]->actionType;
    Draw_in_cellActionDescription* draw_in_cell = actionType != (draw_in_cellAction) ? NULL : (static_cast<Draw_in_cellActionDescription *>(ActionManager::actions[actionId]));
    Detect_board_stateActionDescription* detect_board_state = actionType != (detect_board_stateAction) ? NULL : (static_cast<Detect_board_stateActionDescription *>(ActionManager::actions[actionId]));
    if(!state.isRobotTurn){
int emptyC = 0;
    
for_each(state.grid.begin(),state.grid.end(),[&](int const& cell){emptyC += cell == eEmpty ? 1 : 0;
    });
    
float w = 1.0/emptyC;
    
vector<float> weights{};
    
for(int i=0;
    i< state.grid.size();
    i++)
{
  weights.push_back(state.grid[i] == eEmpty ? w : 0.0);
    
}
int sampledCell = AOSUtils::SampleDiscrete(weights);
    
state_.grid[sampledCell] = eX;
    
state_.isRobotTurn = true;
    };
}

void Iros::ModuleDynamicModel(const IrosState &state, const IrosState &state_, IrosState &state__, double rand_num, int actionId, double &__reward, OBS_TYPE &observation, const int &__moduleExecutionTime, const bool &__meetPrecondition) const
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
        bool success = oCellP != 4 || AOSUtils::Bernoulli(0.5);
        
state__.grid[oCellP] = state__.grid[oCellP] == eEmpty && success ? eO : state__.grid[oCellP];
        
state__.isRobotTurn=!state.isRobotTurn;
        
__moduleResponse= success ? draw_in_cell_res_success : draw_in_cell_res_failed;
        
__reward = 0;
    }
    if(actType == detect_board_stateAction)
    {
        state__.isRobotTurn=!state.isRobotTurn;
        
__moduleResponseStr = "_________";
        
for(int i=0;
        i<state.grid.size();
         i++)
{
   __moduleResponseStr[i] = state__.grid[i] == eX ? 'X' : state__.grid[i] == eO ? 'O' : '?';
        
}
__reward = 0;
    }
    if(__moduleResponseStr != "NoStrResponse")
    {
        IrosResponseModuleAndTempEnums responseHash = (IrosResponseModuleAndTempEnums)hasher(__moduleResponseStr);
        enum_map_iros::vecResponseEnumToString[responseHash] = __moduleResponseStr;
        enum_map_iros::vecStringToResponseEnum[__moduleResponseStr] = responseHash;
        __moduleResponse = responseHash;
    }
}

bool Iros::ProcessSpecialStates(IrosState &state, double &reward) const
{
    float temp_reward = 0;
    bool temp_IsGoalState = false;
    bool temp_StopEvaluatingState = false;
    bool isFinalState = false;
    if(state.OneTimeRewardUsed[0])
    {
        auto stateFunction = [&]()
        {
            float __reward = 0;
            bool __isGoalState = false;
            bool __stopEvaluatingState = false;
            vector<vector<int>> game_end{{8,7,6},{5,4,3},{2,1,0},
                             {8,5,2},{7,4,1},{6,3,0},
                             {8,4,0},{6,4,2}};
            
for(int i =0;
             i < game_end.size();
             i++)
{
int a = game_end[i][0], b = game_end[i][1], c = game_end[i][2];
            
   if(state.grid[a] != eEmpty && state.grid[a] == state.grid[b] && state.grid[b] == state.grid[c])
   {
      __reward = state.grid[a] == eO ? 10 : -10;
            
      __isGoalState=true;
            
      break;
            
   }
}
__isGoalState |= !std::any_of(state.grid.cbegin(), state.grid.cend(), [&](int cell){ return cell == eEmpty;
             });
            return std::make_tuple(__reward, __isGoalState, __stopEvaluatingState);
        };
        std::tie(temp_reward, temp_IsGoalState, temp_StopEvaluatingState) = stateFunction();
        state.OneTimeRewardUsed[0] = !temp_StopEvaluatingState;
        reward += temp_reward;
        isFinalState = temp_IsGoalState ? true : isFinalState;
    }
    return isFinalState;
}





std::string Iros::PrintObs(int action, OBS_TYPE obs) const 
{
	return Prints::PrintObs(action, obs);
}

std::string Iros::PrintStateStr(const State &state) const { return ""; };
}// namespace despot
