#include "example.h"
#include <despot/core/pomdp.h> 
#include <stdlib.h>
#include <despot/solver/pomcp.h>
#include <sstream>
#include <despot/model_primitives/example/actionManager.h> 
#include <despot/model_primitives/example/enum_map_example.h>  
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

Example Example::gen_model;

std::uniform_real_distribution<float> AOSUtils::real_unfirom_dist(0.0,1.0);
vector<vector<float>*> *AOSUtils::streams = new vector<vector<float>*>;
std::default_random_engine AOSUtils::generator(std::random_device{}());
map<int, double> AOSUtils::heuristics_values_cache;
map<int, string> AOSUtils::heuristics_values_cache_checker;

float AOSUtils::GetRandom()
{
    float rand=-1;
    if(Globals::config.stream_id+1 > streams->size())
    {
        AOSUtils::streams->push_back(((new vector<float>)));
        Globals::config.stream_id = streams->size()-1;
    }
    if(Globals::config.stream_step+1 > AOSUtils::streams->at(Globals::config.stream_id)->size())
    {
        rand = real_unfirom_dist(generator);    
        AOSUtils::streams->at(Globals::config.stream_id)->push_back(rand);
    }
    rand = AOSUtils::streams->at(Globals::config.stream_id)->at(Globals::config.stream_step);
    assert(rand >= 0 && rand <= 1);
    Globals::config.stream_step++;
    return rand;
}

int AOSUtils::SampleDiscrete(vector<float> weights)
{
    //float rand = real_unfirom_dist(generator);
    float rand = AOSUtils::GetRandom();
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
    //float rand = real_unfirom_dist(generator);
    float rand = AOSUtils::GetRandom();
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
    //float rand = real_unfirom_dist(generator);
	float rand = AOSUtils::GetRandom();
    return rand < p;
}
/* ==============================================================================
 *ExampleBelief class
 * ==============================================================================*/
int ExampleBelief::num_particles = 50000;
std::string ExampleBelief::beliefFromDB = "";
int ExampleBelief::currentInitParticleIndex = -1;

ExampleBelief::ExampleBelief(vector<State*> particles, const DSPOMDP* model,
	Belief* prior) :
	ParticleBelief(particles, model, prior),
	example_(static_cast<const Example*>(model)) {
}

	
 	std::string Example::GetActionDescription(int actionId) const
	 {
		 return Prints::PrintActionDescription(ActionManager::actions[actionId]);
	 }

void ExampleBelief::Update(int actionId, OBS_TYPE obs, map<std::string, std::string> localVariables) {
	history_.Add(actionId, obs);

    ActionType &actType = ActionManager::actions[actionId]->actionType;
    bool skillSuccess;
    bool goal_reached;
    float nav_to_x;
    float nav_to_y;
    float nav_to_z;



    try
    {
        if(actType == navigateAction)
        {
            if(localVariables.find("skillSuccess") != localVariables.end())
            {
                skillSuccess = localVariables["skillSuccess"] == "true";
            }
            if(localVariables.find("goal_reached") != localVariables.end())
            {
                goal_reached = localVariables["goal_reached"] == "true";
            }
            if(localVariables.find("nav_to_x") != localVariables.end())
            {
                nav_to_x = std::stod(localVariables["nav_to_x"]);
            }
            if(localVariables.find("nav_to_y") != localVariables.end())
            {
                nav_to_y = std::stod(localVariables["nav_to_y"]);
            }
            if(localVariables.find("nav_to_z") != localVariables.end())
            {
                nav_to_z = std::stod(localVariables["nav_to_z"]);
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
		State* particle = example_->Copy(particles_[cur]);
		bool terminal = example_->Step(*particle, Random::RANDOM.NextDouble(),
			actionId, reward, o);
 
		//if (!terminal && o == obs)
        if (o == obs) 
			{
                if(!Globals::IsInternalSimulation())
                {
				ExampleState &state__ = static_cast<ExampleState &>(*particles_[cur]);
                ExampleState &state___ = static_cast<ExampleState &>(*particle);
                    if(actType == navigateAction)
                    {
                        NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
                        tLocation &oDesiredLocation = act.oDesiredLocation;
                    }


                }
				updated.push_back(particle);
		} else {
			example_->Free(particle);
		}

		cur = (cur + 1) % N;

		trials++;
	}

	for (int i = 0; i < particles_.size(); i++)
		example_->Free(particles_[i]);

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
 * ExamplePOMCPPrior class
 * ==============================================================================*/

class ExamplePOMCPPrior: public POMCPPrior {
private:
	const Example* example_;

public:
	ExamplePOMCPPrior(const Example* model) :
		POMCPPrior(model),
		example_(model) {
	}

    double HeuristicValue(const State& state, const State& prev_state)

    {
        string stateStr = Prints::PrintState(state);
        int stateHash = AOSUtils::get_hash(stateStr);
        
        map<int,double>::iterator it =AOSUtils::heuristics_values_cache.find(stateHash);
        double __heuristicValue = -1000000;
        if(it != AOSUtils::heuristics_values_cache.end())
        {
            //element found;
            __heuristicValue = it->second;

            map<int,string>::iterator it2 =AOSUtils::heuristics_values_cache_checker.find(stateHash);
            assert(stateStr.compare(it2->second) == 0);
            Globals::config.heuristic_cache_counter++;
            return __heuristicValue;
        }

        Globals::config.heuristic_counter++;
        
                assert(__heuristicValue != -1000000);//value not set by user
        return __heuristicValue;
    }


	void ComputePreference(const State& state, const DSPOMDP* model) {
		bool print=false;
		const ExampleState& example_state = static_cast<const ExampleState&>(state);
		weighted_preferred_actions_.clear();
        legal_actions_.clear();
		preferred_actions_.clear();
        std::vector<double> weighted_preferred_actions_un_normalized;

        OBS_TYPE t;
		for (int a = 0; a < ActionManager::actions.size(); a++) {
            weighted_preferred_actions_un_normalized.push_back(0);
			double reward = 0;
			bool meetPrecondition = false; 
			Example::CheckPreconditions(example_state, reward, meetPrecondition, a);
            if(meetPrecondition)
            {
                legal_actions_.push_back(a);
                double __heuristicValue =0; 
			Example::ComputePreferredActionValue(example_state, __heuristicValue, a);
   
                weighted_preferred_actions_un_normalized[a]=__heuristicValue;
            }
        }

        double min_h=1000000;
        for (int i = 0; i < legal_actions_.size(); i++)
        {
            int a=legal_actions_[i];
            double t = weighted_preferred_actions_un_normalized[a];
            if(print)      cout << "actionID:" << a << ", " << "heuristicValue:" << weighted_preferred_actions_un_normalized[a]  << endl;
            min_h = min_h < t ? min_h : t;
        }
        min_h *= min_h > 0 ? 0.9 : 1.1;
        double heuristicValueTotal = 0;
        for (int i = 0; i < legal_actions_.size(); i++)
        {
            int a=legal_actions_[i];
            if(weighted_preferred_actions_un_normalized[a] >= min_h)
            {
                weighted_preferred_actions_un_normalized[a] -= min_h;
                heuristicValueTotal += weighted_preferred_actions_un_normalized[a];
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
 * Example class
 * ==============================================================================*/

Example::Example(){
	srand((unsigned int)time(NULL));
}

int Example::NumActions() const {
	return ActionManager::actions.size();
}

double Example::ObsProb(OBS_TYPE obs, const State& state, int actionId) const {
	return 0.9;
}

 


std::default_random_engine Example::generator;


State* Example::CreateStartState(string type) const {
    ExampleState* startState = memory_pool_.Allocate();
    ExampleState& state = *startState;
    state.robotLocation = tLocation();
    state.robotLocation.discrete = -1;
    state.v1 = tVisitedLocation();
    state.v1.discrete = 1;
    state.v2 = tVisitedLocation();
    state.v2.discrete = 2;
    state.v3 = tVisitedLocation();
    state.v3.discrete = 3;
    state.l1 = tLocation();
    state.l1.x = -1.01606154442;
     state.l1.y = 0.660750925541;
     state.l1.z =-0.00454711914062;
     state.l1.discrete = 1;
    state.l2 = tLocation();
    state.l2.x = 0.00500533776358;
     state.l2.y = 0.640727937222;
     state.l2.z =-0.00143432617188;
     state.l2.discrete = 2;
    state.l3 = tLocation();
    state.l3.x = 0.986030161381;
     state.l3.y = 0.610693752766;
     state.l3.z =-0.00143432617188;
     state.l3.discrete = 3;
    startState->tLocationObjects.push_back(&(state.robotLocation));
    startState->tVisitedLocationObjects.push_back(&(state.v1));
    startState->tVisitedLocationObjects.push_back(&(state.v2));
    startState->tVisitedLocationObjects.push_back(&(state.v3));
    startState->tLocationObjectsForActions["state.l1"] = (state.l1);
    startState->tLocationObjectsForActions["state.l2"] = (state.l2);
    startState->tLocationObjectsForActions["state.l3"] = (state.l3);
    state.robotLocation.discrete = AOSUtils::Bernoulli(0.5) ? 1 : (AOSUtils::Bernoulli(0.2) ? 2 : 3);
    if (ActionManager::actions.size() == 0)
    {
        ActionManager::Init(const_cast <ExampleState*> (startState));
    }
    double r;
    state.__isTermianl = ProcessSpecialStates(state, r);
    return startState;
}




Belief* Example::InitialBelief(const State* start, string type) const {
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
	int N = ExampleBelief::num_particles;
	vector<State*> particles(N);
	for (int i = 0; i < N; i++) {
        ExampleBelief::currentInitParticleIndex = i;
		particles[i] = CreateStartState();
		particles[i]->weight = 1.0 / N;
	}
    ExampleBelief::currentInitParticleIndex = -1;
	return new ExampleBelief(particles, this);
}
 

 

POMCPPrior* Example::CreatePOMCPPrior(string name) const { 
		return new ExamplePOMCPPrior(this);
}

void Example::PrintState(const State& state, ostream& ostr) const {
	const ExampleState& farstate = static_cast<const ExampleState&>(state);
	if (ostr)
		ostr << Prints::PrintState(farstate);
}

void Example::PrintObs(const State& state, OBS_TYPE observation,
	ostream& ostr) const {
	const ExampleState& farstate = static_cast<const ExampleState&>(state);
	
	ostr << observation <<endl;
}

void Example::PrintBelief(const Belief& belief, ostream& out) const {
	 out << "called PrintBelief(): b printed"<<endl;
		out << endl;
	
}

void Example::PrintAction(int actionId, ostream& out) const {
	out << Prints::PrintActionDescription(ActionManager::actions[actionId]) << endl;
}

State* Example::Allocate(int state_id, double weight) const {
	ExampleState* state = memory_pool_.Allocate();
	state->state_id = state_id;
	state->weight = weight;
	return state;
}

State* Example::Copy(const State* particle) const {
	ExampleState* state = memory_pool_.Allocate();
	*state = *static_cast<const ExampleState*>(particle);
	state->SetAllocated();

    state->tLocationObjects[0] = &(state->robotLocation);
    state->tVisitedLocationObjects[0] = &(state->v1);
    state->tVisitedLocationObjects[1] = &(state->v2);
    state->tVisitedLocationObjects[2] = &(state->v3);


	return state;
}

void Example::Free(State* particle) const {
	memory_pool_.Free(static_cast<ExampleState*>(particle));
}

int Example::NumActiveParticles() const {
	return memory_pool_.num_allocated();
}

int Example::GetStateHash(State state) const
{ 
    return State::GetStateHash(state);
}

void Example::StepForModel(State& state, int actionId, double& reward,
        OBS_TYPE& observation, int &state_hash, int &next_state_hash, bool& isTerminal, double& precondition_reward, double& specialStateReward) const
    {
        reward = 0;
        //ExampleState &ir_state = static_cast<ExampleState &>(state);
        state_hash = GetStateHash(state);

        bool meetPrecondition;
        precondition_reward = 0;
        CheckPreconditions(state, reward, meetPrecondition, actionId);
        if(!meetPrecondition)
        {
            precondition_reward = reward;
        }
        
        isTerminal = Example::Step(state, 0.1, actionId, reward,
                   observation);
        //ir_state = static_cast<ExampleState &>(state);

        specialStateReward = 0;
        ProcessSpecialStates(state, specialStateReward);
        reward -= (precondition_reward + specialStateReward);//so that it will not consider the precondition penalty and special states reward

        next_state_hash = GetStateHash(state);
    }
bool Example::Step(State& s_state__, double rand_num, int actionId, double& reward,
	OBS_TYPE& observation) const {
    Globals::config.steps_counter++;
    observation = default_moduleResponse;
    reward = 0;
	Random random(rand_num);
	int __moduleExecutionTime = -1;
	bool meetPrecondition = false;
	double tReward = 0;

	ExampleState &state__ = static_cast<ExampleState &>(s_state__);
	 logd << "[Example::Step] Selected Action:" << Prints::PrintActionDescription(ActionManager::actions[actionId]) << "||State"<< Prints::PrintState(state__);
	CheckPreconditions(state__, reward, meetPrecondition, actionId);
	State *s_state = Copy(&s_state__);
	ExampleState &state = static_cast<ExampleState &>(*s_state);

	
	SampleModuleExecutionTime(state__, rand_num, actionId, __moduleExecutionTime);

	ExtrinsicChangesDynamicModel(state, state__, rand_num, actionId, __moduleExecutionTime, tReward);
    reward += tReward;
    tReward = 0;
    

	State *s_state_ = Copy(&s_state__);
	ExampleState &state_ = static_cast<ExampleState &>(*s_state_);

    
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

void Example::CheckPreconditions(const ExampleState& state, double &reward, bool &__meetPrecondition, int actionId)
    {
        ActionType &actType = ActionManager::actions[actionId]->actionType;
        __meetPrecondition = true;
            if(actType == navigateAction)
            {
                NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
                tLocation &oDesiredLocation = act.oDesiredLocation;
                __meetPrecondition = oDesiredLocation.discrete != state.robotLocation.discrete;
                if(!__meetPrecondition) reward += -10;
            }
    }

void Example::ComputePreferredActionValue(const ExampleState& state, double &__heuristicValue, int actionId)
    {
        __heuristicValue = 0;
        ActionType &actType = ActionManager::actions[actionId]->actionType;
            if(actType == navigateAction)
            {
                NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
                tLocation &oDesiredLocation = act.oDesiredLocation;
            }
        __heuristicValue = __heuristicValue < 0 ? 0 : __heuristicValue;
    }

void Example::SampleModuleExecutionTime(const ExampleState& farstate, double rand_num, int actionId, int &__moduleExecutionTime) const
{
    ActionType &actType = ActionManager::actions[actionId]->actionType;
    if(actType == navigateAction)
    {
    }
}

void Example::ExtrinsicChangesDynamicModel(const ExampleState& state, ExampleState& state_, double rand_num, int actionId, const int &__moduleExecutionTime,  double &__reward)  const
{
    ActionType &actionType = ActionManager::actions[actionId]->actionType;
    NavigateActionDescription* navigate = actionType != (navigateAction) ? NULL : (static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
    if (AOSUtils::Bernoulli(0.05)) state_.robotLocation.discrete = -1;
    

;
}

void Example::ModuleDynamicModel(const ExampleState &state, const ExampleState &state_, ExampleState &state__, double rand_num, int actionId, double &__reward, OBS_TYPE &observation, const int &__moduleExecutionTime, const bool &__meetPrecondition) const
{
    std::hash<std::string> hasher;
    ActionType &actType = ActionManager::actions[actionId]->actionType;
    observation = -1;
    observation = default_moduleResponse;
    std::string __moduleResponseStr = "NoStrResponse";
    OBS_TYPE &__moduleResponse = observation;
    if(actType == navigateAction)
    {
        NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
        tLocation &oDesiredLocation = act.oDesiredLocation;
        state__.robotLocation.discrete = ! __meetPrecondition || AOSUtils::Bernoulli(0.1) ? -1: oDesiredLocation.discrete;
        
if(state__.robotLocation.discrete == oDesiredLocation.discrete)
{ 
state__.robotLocation.x = oDesiredLocation.x;
         
state__.robotLocation.y = oDesiredLocation.y;
         
state__.robotLocation.z = oDesiredLocation.z;
        
}
for(int i=0;
         i < state__.tVisitedLocationObjects.size();
        i++)
{
    if(state__.tVisitedLocationObjects[i]->discrete == state__.robotLocation.discrete) 
    {
    state__.tVisitedLocationObjects[i]->visited = true;
        
    break;
        
    }
}
__moduleResponse = (state__.robotLocation.discrete == -1 && AOSUtils::Bernoulli(0.8)) ? navigate_eFailed : navigate_eSuccess;
        
__reward = state_.robotLocation.discrete == -1 ? -5 : -(sqrt(pow(state.robotLocation.x-oDesiredLocation.x,2.0)+pow(state.robotLocation.y-oDesiredLocation.y,2.0)))*100;
        
if (state__.robotLocation.discrete == -1) __reward =  -10;
    }
    if(__moduleResponseStr != "NoStrResponse")
    {
        ExampleResponseModuleAndTempEnums responseHash = (ExampleResponseModuleAndTempEnums)AOSUtils::get_hash(__moduleResponseStr);
        enum_map_example::vecResponseEnumToString[responseHash] = __moduleResponseStr;
        enum_map_example::vecStringToResponseEnum[__moduleResponseStr] = responseHash;
        __moduleResponse = responseHash;
    }
}

bool Example::ProcessSpecialStates(ExampleState &state, double &reward) const
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
            if (!state.v1.visited && state.v2.visited)
{
__reward=-50;
            
__stopEvaluatingState =true;
            
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
            if (state.v1.visited && state.v2.visited && state.v3.visited)
{
__reward =7000;
            
__isGoalState =true;
            
};
            return std::make_tuple(__reward, __isGoalState, __stopEvaluatingState);
        };
        std::tie(temp_reward, temp_IsGoalState, temp_StopEvaluatingState) = stateFunction();
        state.OneTimeRewardUsed[1] = !temp_StopEvaluatingState;
        reward += temp_reward;
        isFinalState = temp_IsGoalState ? true : isFinalState;
    }
    return isFinalState;
}





std::string Example::PrintObs(int action, OBS_TYPE obs) const 
{
	return Prints::PrintObs(obs);
}

std::string Example::PrintStateStr(const State &state) const { return ""; };
}// namespace despot
