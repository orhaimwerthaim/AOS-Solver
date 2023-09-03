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
#include <filesystem>
namespace fs = std::filesystem;
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

int AOSUtils::get_hash(string str_)
    {
        const char *str = str_.c_str();
        unsigned long hash = 0;
        int c;

        while (c = *str++)
            hash = c + (hash << 6) + (hash << 16) - hash;

        return hash; 
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
/* ==============================================================================
 *IrosBelief class
 * ==============================================================================*/
int IrosBelief::num_particles = 20000;
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



    try
    {
        if(actType == detect_board_stateAction)
        {
            if(localVariables.find("obsr") != localVariables.end())
            {
                obsr = localVariables["obsr"];
            }
        }
        if(actType == draw_in_cellAction)
        {
            if(localVariables.find("cell_to_mark") != localVariables.end())
            {
                cell_to_mark = std::stoi(localVariables["cell_to_mark"]);
            }
        }

    }
    catch(const std::exception& e)
    {
        std::string s ="Error: problem loading LocalVariables data for belief state update. ";
        MongoDB_Bridge::AddLog(s + e.what(), eLogLevel::ERROR);
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
                    if(actType == detect_board_stateAction)
                    {
                    }
                    if(actType == draw_in_cellAction)
                    {
                        Draw_in_cellActionDescription act = *(static_cast<Draw_in_cellActionDescription *>(ActionManager::actions[actionId]));
                        int &oCellP = act.oCellP;
                    }


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
    
     if(Globals::config.verbosity >= eLogLevel::FATAL && particles_.size() == 0)
 {
    std::string logMsg("Could not update belief after action: " + Prints::PrintActionDescription(actionId) +". Received observation:" + Prints::PrintObs(obs));
    MongoDB_Bridge::AddLog(logMsg, eLogLevel::FATAL);
 }
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
		for (int a = 0; a < ActionManager::actions.size(); a++) {
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
            for (int a = 0; a < ActionManager::actions.size(); a++) 
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
    if(Globals::config.solveProblemWithClosedPomdpModel  && !Globals::config.useSavedSarsopPolicy)
    {
        bool best_policy_exists = false;
        char tmp[256];
		getcwd(tmp, 256);
		std::string workingDirPath(tmp);
		workingDirPath = workingDirPath.substr(0, workingDirPath.find("build"));
        workingDirPath.append("sarsop/src/");
		std::string metadata_fpath(workingDirPath);
        metadata_fpath.append("policy_metadata.txt");
        ifstream meta_fs(metadata_fpath.c_str());
        std::string hash; 
        int search_depth;
        string desc1, desc2; 
		// std::string state_map( (std::istreambuf_iterator<char>(meta_fs) ),
        //                (std::istreambuf_iterator<char>()    ) );
		
        meta_fs >> desc1 >> hash >> desc2 >> search_depth;
        if(Globals::config.domainHash == hash && search_depth == Globals::config.search_depth)
        {
            for (const auto & entry : fs::directory_iterator(workingDirPath))
            {
                string f_name(entry.path().u8string());
                f_name = f_name.substr(workingDirPath.size());
                if (f_name == "out.policy")
                {
                    best_policy_exists=true;
                    break;
                }
            }
        }

        if(!best_policy_exists)
        {
            
        }
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

int Iros::GetStateHash(State state) const
{ 
    return State::GetStateHash(state);
}

void Iros::StepForModel(State& state, int actionId, double& reward,
        OBS_TYPE& observation, int &state_hash, int &next_state_hash, bool& isTerminal, double& precondition_reward, double& specialStateReward) const
    {
        reward = 0;
        //IrosState &ir_state = static_cast<IrosState &>(state);
        state_hash = GetStateHash(state);

        bool meetPrecondition;
        precondition_reward = 0;
        CheckPreconditions(state, reward, meetPrecondition, actionId);
        if(!meetPrecondition)
        {
            precondition_reward = reward;
        }
        
        isTerminal = Iros::Step(state, 0.1, actionId, reward,
                   observation);
        //ir_state = static_cast<IrosState &>(state);

        specialStateReward = 0;
        ProcessSpecialStates(state, specialStateReward);
        reward -= (precondition_reward + specialStateReward);//so that it will not consider the precondition penalty and special states reward

        next_state_hash = GetStateHash(state);
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
            if(actType == detect_board_stateAction)
            {
                __meetPrecondition=!state.isRobotTurn;
                if(!__meetPrecondition) reward += -10;
            }
            if(actType == draw_in_cellAction)
            {
                Draw_in_cellActionDescription act = *(static_cast<Draw_in_cellActionDescription *>(ActionManager::actions[actionId]));
                int &oCellP = act.oCellP;
                __meetPrecondition=state.isRobotTurn && state.grid[oCellP] == eEmpty;
                if(!__meetPrecondition) reward += -10;
            }
    }

void Iros::ComputePreferredActionValue(const IrosState& state, double &__heuristicValue, int actionId)
    {
        __heuristicValue = 0;
        ActionType &actType = ActionManager::actions[actionId]->actionType;
            if(actType == detect_board_stateAction)
            {
            }
            if(actType == draw_in_cellAction)
            {
                Draw_in_cellActionDescription act = *(static_cast<Draw_in_cellActionDescription *>(ActionManager::actions[actionId]));
                int &oCellP = act.oCellP;
            }
        __heuristicValue = __heuristicValue < 0 ? 0 : __heuristicValue;
    }

void Iros::SampleModuleExecutionTime(const IrosState& farstate, double rand_num, int actionId, int &__moduleExecutionTime) const
{
    ActionType &actType = ActionManager::actions[actionId]->actionType;
    if(actType == detect_board_stateAction)
    {
    }
    if(actType == draw_in_cellAction)
    {
    }
}

void Iros::ExtrinsicChangesDynamicModel(const IrosState& state, IrosState& state_, double rand_num, int actionId, const int &__moduleExecutionTime,  double &__reward)  const
{
    ActionType &actionType = ActionManager::actions[actionId]->actionType;
    Detect_board_stateActionDescription* detect_board_state = actionType != (detect_board_stateAction) ? NULL : (static_cast<Detect_board_stateActionDescription *>(ActionManager::actions[actionId]));
    Draw_in_cellActionDescription* draw_in_cell = actionType != (draw_in_cellAction) ? NULL : (static_cast<Draw_in_cellActionDescription *>(ActionManager::actions[actionId]));
    if(!state.isRobotTurn){
int emptyC = 0;
    
for_each(state.grid.begin(),state.grid.end(),[&](int const& cell){emptyC += cell == eEmpty ? 1 : 0;
    });
    
float w = 1.0/emptyC;
    
vector<float> weights{};
    
for(int i=0;
    i< 10;
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
    if(actType == draw_in_cellAction)
    {
        Draw_in_cellActionDescription act = *(static_cast<Draw_in_cellActionDescription *>(ActionManager::actions[actionId]));
        int &oCellP = act.oCellP;
        if(state.isRobotTurn){
state__.grid[oCellP] = state__.grid[oCellP] == eEmpty ? eO : state__.grid[oCellP];
        
state__.isRobotTurn=!state.isRobotTurn;
        }
__moduleResponse= draw_in_cell_res_success;
        
__reward = 0;
    }
    if(__moduleResponseStr != "NoStrResponse")
    {
        IrosResponseModuleAndTempEnums responseHash = (IrosResponseModuleAndTempEnums)AOSUtils::get_hash(__moduleResponseStr);
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
            if(state.grid[8] == state.grid[7]){__stopEvaluatingState = true;
             __reward = state.grid[8] == eO ? 1 : -1;
            };
            return std::make_tuple(__reward, __isGoalState, __stopEvaluatingState);
        };
        std::tie(temp_reward, temp_IsGoalState, temp_StopEvaluatingState) = stateFunction();
        state.OneTimeRewardUsed[0] = !temp_StopEvaluatingState;
        reward += temp_reward;
        isFinalState = temp_IsGoalState ? true : isFinalState;
    }
    if(state.OneTimeRewardUsed[1])
    {
        auto stateFunction = [&]()
        {
            float __reward = 0;
            bool __isGoalState = false;
            bool __stopEvaluatingState = false;
            if(state.grid[8] == state.grid[5]){__stopEvaluatingState = true;
             __reward = state.grid[8] == eO ? 1 : -1;
            };
            return std::make_tuple(__reward, __isGoalState, __stopEvaluatingState);
        };
        std::tie(temp_reward, temp_IsGoalState, temp_StopEvaluatingState) = stateFunction();
        state.OneTimeRewardUsed[1] = !temp_StopEvaluatingState;
        reward += temp_reward;
        isFinalState = temp_IsGoalState ? true : isFinalState;
    }
    if(state.OneTimeRewardUsed[2])
    {
        auto stateFunction = [&]()
        {
            float __reward = 0;
            bool __isGoalState = false;
            bool __stopEvaluatingState = false;
            if(state.grid[6] == state.grid[7]){__stopEvaluatingState = true;
             __reward = state.grid[6] == eO ? 1 : -1;
            };
            return std::make_tuple(__reward, __isGoalState, __stopEvaluatingState);
        };
        std::tie(temp_reward, temp_IsGoalState, temp_StopEvaluatingState) = stateFunction();
        state.OneTimeRewardUsed[2] = !temp_StopEvaluatingState;
        reward += temp_reward;
        isFinalState = temp_IsGoalState ? true : isFinalState;
    }
    if(state.OneTimeRewardUsed[3])
    {
        auto stateFunction = [&]()
        {
            float __reward = 0;
            bool __isGoalState = false;
            bool __stopEvaluatingState = false;
            if(state.grid[6] == state.grid[3]){__stopEvaluatingState = true;
             __reward = state.grid[6] == eO ? 1 : -1;
            };
            return std::make_tuple(__reward, __isGoalState, __stopEvaluatingState);
        };
        std::tie(temp_reward, temp_IsGoalState, temp_StopEvaluatingState) = stateFunction();
        state.OneTimeRewardUsed[3] = !temp_StopEvaluatingState;
        reward += temp_reward;
        isFinalState = temp_IsGoalState ? true : isFinalState;
    }
    if(state.OneTimeRewardUsed[4])
    {
        auto stateFunction = [&]()
        {
            float __reward = 0;
            bool __isGoalState = false;
            bool __stopEvaluatingState = false;
            if(state.grid[2] == state.grid[5]){__stopEvaluatingState = true;
             __reward = state.grid[2] == eO ? 1 : -1;
            };
            return std::make_tuple(__reward, __isGoalState, __stopEvaluatingState);
        };
        std::tie(temp_reward, temp_IsGoalState, temp_StopEvaluatingState) = stateFunction();
        state.OneTimeRewardUsed[4] = !temp_StopEvaluatingState;
        reward += temp_reward;
        isFinalState = temp_IsGoalState ? true : isFinalState;
    }
    if(state.OneTimeRewardUsed[5])
    {
        auto stateFunction = [&]()
        {
            float __reward = 0;
            bool __isGoalState = false;
            bool __stopEvaluatingState = false;
            if(state.grid[2] == state.grid[1]){__stopEvaluatingState = true;
             __reward = state.grid[2] == eO ? 1 : -1;
            };
            return std::make_tuple(__reward, __isGoalState, __stopEvaluatingState);
        };
        std::tie(temp_reward, temp_IsGoalState, temp_StopEvaluatingState) = stateFunction();
        state.OneTimeRewardUsed[5] = !temp_StopEvaluatingState;
        reward += temp_reward;
        isFinalState = temp_IsGoalState ? true : isFinalState;
    }
    if(state.OneTimeRewardUsed[6])
    {
        auto stateFunction = [&]()
        {
            float __reward = 0;
            bool __isGoalState = false;
            bool __stopEvaluatingState = false;
            if(state.grid[0] == state.grid[1]){__stopEvaluatingState = true;
             __reward = state.grid[0] == eO ? 1 : -1;
            };
            return std::make_tuple(__reward, __isGoalState, __stopEvaluatingState);
        };
        std::tie(temp_reward, temp_IsGoalState, temp_StopEvaluatingState) = stateFunction();
        state.OneTimeRewardUsed[6] = !temp_StopEvaluatingState;
        reward += temp_reward;
        isFinalState = temp_IsGoalState ? true : isFinalState;
    }
    if(state.OneTimeRewardUsed[7])
    {
        auto stateFunction = [&]()
        {
            float __reward = 0;
            bool __isGoalState = false;
            bool __stopEvaluatingState = false;
            if(state.grid[0] == state.grid[3]){__stopEvaluatingState = true;
             __reward = state.grid[0] == eO ? 1 : -1;
            };
            return std::make_tuple(__reward, __isGoalState, __stopEvaluatingState);
        };
        std::tie(temp_reward, temp_IsGoalState, temp_StopEvaluatingState) = stateFunction();
        state.OneTimeRewardUsed[7] = !temp_StopEvaluatingState;
        reward += temp_reward;
        isFinalState = temp_IsGoalState ? true : isFinalState;
    }
    if(state.OneTimeRewardUsed[8])
    {
        auto stateFunction = [&]()
        {
            float __reward = 0;
            bool __isGoalState = false;
            bool __stopEvaluatingState = false;
            __isGoalState |= !std::any_of(state.grid.cbegin(), state.grid.cend(), [&](int cell){ return cell == eEmpty;
             });
            return std::make_tuple(__reward, __isGoalState, __stopEvaluatingState);
        };
        std::tie(temp_reward, temp_IsGoalState, temp_StopEvaluatingState) = stateFunction();
        state.OneTimeRewardUsed[8] = !temp_StopEvaluatingState;
        reward += temp_reward;
        isFinalState = temp_IsGoalState ? true : isFinalState;
    }
    return isFinalState;
}





std::string Iros::PrintObs(int action, OBS_TYPE obs) const 
{
	return Prints::PrintObs(obs);
}

std::string Iros::PrintStateStr(const State &state) const { return ""; };
}// namespace despot
