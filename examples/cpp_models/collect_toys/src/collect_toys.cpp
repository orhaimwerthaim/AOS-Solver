#include "collect_toys.h"
#include <despot/core/pomdp.h> 
#include <stdlib.h>
#include <despot/solver/pomcp.h>
#include <sstream>
#include <despot/model_primitives/collect_toys/actionManager.h> 
#include <despot/model_primitives/collect_toys/enum_map_collect_toys.h>  
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

Collect_toys Collect_toys::gen_model;

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
 *Collect_toysBelief class
 * ==============================================================================*/
int Collect_toysBelief::num_particles = 20000;
std::string Collect_toysBelief::beliefFromDB = "";
int Collect_toysBelief::currentInitParticleIndex = -1;

Collect_toysBelief::Collect_toysBelief(vector<State*> particles, const DSPOMDP* model,
	Belief* prior) :
	ParticleBelief(particles, model, prior),
	collect_toys_(static_cast<const Collect_toys*>(model)) {
}

	
 	std::string Collect_toys::GetActionDescription(int actionId) const
	 {
		 return Prints::PrintActionDescription(ActionManager::actions[actionId]);
	 }

void Collect_toysBelief::Update(int actionId, OBS_TYPE obs, map<std::string, std::string> localVariables) {
	history_.Add(actionId, obs);

    ActionType &actType = ActionManager::actions[actionId]->actionType;
    bool skillSuccess3;
    int destination;
    bool skillSuccess2;
    string toy_type_name;
    bool skillSuccess;
    string obsr;



    try
    {
        if(actType == detectAction)
        {
            if(localVariables.find("obsr") != localVariables.end())
            {
                obsr = localVariables["obsr"];
            }
        }
        if(actType == placeAction)
        {
            if(localVariables.find("skillSuccess3") != localVariables.end())
            {
                skillSuccess3 = localVariables["skillSuccess3"] == "true";
            }
        }
        if(actType == pickAction)
        {
            if(localVariables.find("toy_type_name") != localVariables.end())
            {
                toy_type_name = localVariables["toy_type_name"];
            }
            if(localVariables.find("skillSuccess") != localVariables.end())
            {
                skillSuccess = localVariables["skillSuccess"] == "true";
            }
        }
        if(actType == navigateAction)
        {
            if(localVariables.find("destination") != localVariables.end())
            {
                destination = std::stoi(localVariables["destination"]);
            }
            if(localVariables.find("skillSuccess2") != localVariables.end())
            {
                skillSuccess2 = localVariables["skillSuccess2"] == "true";
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
		State* particle = collect_toys_->Copy(particles_[cur]);
		bool terminal = collect_toys_->Step(*particle, Random::RANDOM.NextDouble(),
			actionId, reward, o);
 
		//if (!terminal && o == obs)
        if (o == obs) 
			{
                if(!Globals::IsInternalSimulation())
                {
				Collect_toysState &state__ = static_cast<Collect_toysState &>(*particles_[cur]);
                Collect_toysState &state___ = static_cast<Collect_toysState &>(*particle);
                    if(actType == detectAction)
                    {
                    }
                    if(actType == placeAction)
                    {
                    }
                    if(actType == pickAction)
                    {
                        PickActionDescription act = *(static_cast<PickActionDescription *>(ActionManager::actions[actionId]));
                        string &toyType = act.toyType;
                    }
                    if(actType == navigateAction)
                    {
                        NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
                        int &oDestination = act.oDestination;
                    }


                }
				updated.push_back(particle);
		} else {
			collect_toys_->Free(particle);
		}

		cur = (cur + 1) % N;

		trials++;
	}

	for (int i = 0; i < particles_.size(); i++)
		collect_toys_->Free(particles_[i]);

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
 * Collect_toysPOMCPPrior class
 * ==============================================================================*/

class Collect_toysPOMCPPrior: public POMCPPrior {
private:
	const Collect_toys* collect_toys_;

public:
	Collect_toysPOMCPPrior(const Collect_toys* model) :
		POMCPPrior(model),
		collect_toys_(model) {
	}

	void ComputePreference(const State& state) {
		const Collect_toysState& collect_toys_state = static_cast<const Collect_toysState&>(state);
		weighted_preferred_actions_.clear();
        legal_actions_.clear();
		preferred_actions_.clear();
        std::vector<double> weighted_preferred_actions_un_normalized;

        double heuristicValueTotal = 0;
		for (int a = 0; a < ActionManager::actions.size(); a++) {
            weighted_preferred_actions_un_normalized.push_back(0);
			double reward = 0;
			bool meetPrecondition = false; 
			Collect_toys::CheckPreconditions(collect_toys_state, reward, meetPrecondition, a);
            if(meetPrecondition)
            {
                legal_actions_.push_back(a);
                double __heuristicValue; 
                Collect_toys::ComputePreferredActionValue(collect_toys_state, __heuristicValue, a);
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
 * Collect_toys class
 * ==============================================================================*/

Collect_toys::Collect_toys(){
	srand((unsigned int)time(NULL));
}

int Collect_toys::NumActions() const {
	return ActionManager::actions.size();
}

double Collect_toys::ObsProb(OBS_TYPE obs, const State& state, int actionId) const {
	return 0.9;
}

 


std::default_random_engine Collect_toys::generator;


State* Collect_toys::CreateStartState(string type) const {
    Collect_toysState* startState = memory_pool_.Allocate();
    Collect_toysState& state = *startState;
    startState->eLocationObjects.push_back(L0);
    startState->eLocationObjects.push_back(L1);
    startState->eLocationObjects.push_back(L2);
    startState->eLocationObjects.push_back(L3);
    startState->eLocationObjects.push_back(LChild);
    startState->eLocationObjects.push_back(LArm);
    state.tGreen = tToy();
    state.tGreen.type="green";
    state.tGreen.reward=40;
    state.observed_reward = true;
    state.tBlue = tToy();
    state.tBlue.type="blue";
    state.tBlue.reward=20;
    state.tBlack = tToy();
    state.tBlack.type="black";
    state.tBlack.reward=10;
    state.tRed = tToy();
    state.tRed.type="red";
    state.tRed.reward=10;
    state.pickActionsLeft=12;
    state.robotLocation=LChild;
    startState->tToyObjects.push_back(&(state.tGreen));
    startState->tToyObjects.push_back(&(state.tBlue));
    startState->tToyObjects.push_back(&(state.tBlack));
    startState->tToyObjects.push_back(&(state.tRed));
    vector<float> weights1{0.1,0.2,0.5,0.1};
    
state.tGreen.location = AOSUtils::SampleDiscrete(weights1);
     

vector<float> weights2{0.5,0.2,0.0,0.3};
    
state.tBlue.location = AOSUtils::SampleDiscrete(weights2);
     

vector<float> weights3{0.1,0.5,0.0,0.4};
    
state.tBlack.location = AOSUtils::SampleDiscrete(weights3);
     

vector<float> weights4{0.2,0.3,0.1,0.4};
    
state.tRed.location = AOSUtils::SampleDiscrete(weights4);
     
;
    if (ActionManager::actions.size() == 0)
    {
        ActionManager::Init(const_cast <Collect_toysState*> (startState));
    }
    double r;
    state.__isTermianl = ProcessSpecialStates(state, r);
    return startState;
}




Belief* Collect_toys::InitialBelief(const State* start, string type) const {
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
	int N = Collect_toysBelief::num_particles;
	vector<State*> particles(N);
	for (int i = 0; i < N; i++) {
        Collect_toysBelief::currentInitParticleIndex = i;
		particles[i] = CreateStartState();
		particles[i]->weight = 1.0 / N;
	}
    Collect_toysBelief::currentInitParticleIndex = -1;
	return new Collect_toysBelief(particles, this);
}
 

 

POMCPPrior* Collect_toys::CreatePOMCPPrior(string name) const { 
		return new Collect_toysPOMCPPrior(this);
}

void Collect_toys::PrintState(const State& state, ostream& ostr) const {
	const Collect_toysState& farstate = static_cast<const Collect_toysState&>(state);
	if (ostr)
		ostr << Prints::PrintState(farstate);
}

void Collect_toys::PrintObs(const State& state, OBS_TYPE observation,
	ostream& ostr) const {
	const Collect_toysState& farstate = static_cast<const Collect_toysState&>(state);
	
	ostr << observation <<endl;
}

void Collect_toys::PrintBelief(const Belief& belief, ostream& out) const {
	 out << "called PrintBelief(): b printed"<<endl;
		out << endl;
	
}

void Collect_toys::PrintAction(int actionId, ostream& out) const {
	out << Prints::PrintActionDescription(ActionManager::actions[actionId]) << endl;
}

State* Collect_toys::Allocate(int state_id, double weight) const {
	Collect_toysState* state = memory_pool_.Allocate();
	state->state_id = state_id;
	state->weight = weight;
	return state;
}

State* Collect_toys::Copy(const State* particle) const {
	Collect_toysState* state = memory_pool_.Allocate();
	*state = *static_cast<const Collect_toysState*>(particle);
	state->SetAllocated();

    state->tToyObjects[0] = &(state->tGreen);
    state->tToyObjects[1] = &(state->tBlue);
    state->tToyObjects[2] = &(state->tBlack);
    state->tToyObjects[3] = &(state->tRed);


	return state;
}

void Collect_toys::Free(State* particle) const {
	memory_pool_.Free(static_cast<Collect_toysState*>(particle));
}

int Collect_toys::NumActiveParticles() const {
	return memory_pool_.num_allocated();
}

int Collect_toys::GetStateHash(State state) const
{ 
    return State::GetStateHash(state);
}

void Collect_toys::StepForModel(State& state, int actionId, double& reward,
        OBS_TYPE& observation, int &state_hash, int &next_state_hash, bool& isTerminal, double& precondition_reward, double& specialStateReward) const
    {
        reward = 0;
        //Collect_toysState &ir_state = static_cast<Collect_toysState &>(state);
        state_hash = GetStateHash(state);

        bool meetPrecondition;
        precondition_reward = 0;
        CheckPreconditions(state, reward, meetPrecondition, actionId);
        if(!meetPrecondition)
        {
            precondition_reward = reward;
        }
        
        isTerminal = Collect_toys::Step(state, 0.1, actionId, reward,
                   observation);
        //ir_state = static_cast<Collect_toysState &>(state);

        specialStateReward = 0;
        ProcessSpecialStates(state, specialStateReward);
        reward -= (precondition_reward + specialStateReward);//so that it will not consider the precondition penalty and special states reward

        next_state_hash = GetStateHash(state);
    }
bool Collect_toys::Step(State& s_state__, double rand_num, int actionId, double& reward,
	OBS_TYPE& observation) const {
    observation = default_moduleResponse;
    reward = 0;
	Random random(rand_num);
	int __moduleExecutionTime = -1;
	bool meetPrecondition = false;
	double tReward = 0;

	Collect_toysState &state__ = static_cast<Collect_toysState &>(s_state__);
	 logd << "[Collect_toys::Step] Selected Action:" << Prints::PrintActionDescription(ActionManager::actions[actionId]) << "||State"<< Prints::PrintState(state__);
	CheckPreconditions(state__, reward, meetPrecondition, actionId);
	State *s_state = Copy(&s_state__);
	Collect_toysState &state = static_cast<Collect_toysState &>(*s_state);

	
	SampleModuleExecutionTime(state__, rand_num, actionId, __moduleExecutionTime);

	ExtrinsicChangesDynamicModel(state, state__, rand_num, actionId, __moduleExecutionTime, tReward);
    reward += tReward;
    tReward = 0;
    

	State *s_state_ = Copy(&s_state__);
	Collect_toysState &state_ = static_cast<Collect_toysState &>(*s_state_);

    
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

void Collect_toys::CheckPreconditions(const Collect_toysState& state, double &reward, bool &__meetPrecondition, int actionId)
    {
        ActionType &actType = ActionManager::actions[actionId]->actionType;
        __meetPrecondition = true;
            if(actType == detectAction)
            {
                if(!__meetPrecondition) reward += 0;
            }
            if(actType == placeAction)
            {
                __meetPrecondition = false;
                
for(int i=0;
                 i < state.tToyObjects.size();
                 i++)
{
    if(LArm == (*state.tToyObjects[i]).location) 
    {
        __meetPrecondition = true;
                 break;
                
    }
};
                if(!__meetPrecondition) reward += 0;
            }
            if(actType == pickAction)
            {
                PickActionDescription act = *(static_cast<PickActionDescription *>(ActionManager::actions[actionId]));
                string &toyType = act.toyType;
                __meetPrecondition = false;
                
bool holding = false;
                
bool typeMatch = false;
                
for(int i=0;
                 i < state.tToyObjects.size();
                 i++)
{
    if(LArm == state.tToyObjects[i]->location)
    { 
        holding = true;
                 break;
                
    }
    if(state.robotLocation == state.tToyObjects[i]->location && state.tToyObjects[i]->type == toyType) typeMatch=true;
                
}
__meetPrecondition = !holding && typeMatch && state.robotLocation != LChild;
                if(!__meetPrecondition) reward += 0;
            }
            if(actType == navigateAction)
            {
                NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
                int &oDestination = act.oDestination;
                if(!__meetPrecondition) reward += 0;
            }
    }

void Collect_toys::ComputePreferredActionValue(const Collect_toysState& state, double &__heuristicValue, int actionId)
    {
        __heuristicValue = 0;
        ActionType &actType = ActionManager::actions[actionId]->actionType;
            if(actType == detectAction)
            {
            }
            if(actType == placeAction)
            {
                for(int i=0;
                 i < state.tToyObjects.size();
                 i++)
{
     if(LArm == (*state.tToyObjects[i]).location && state.robotLocation == LChild)
     { 
     __heuristicValue=100;
                 break;
                
     }
};
            }
            if(actType == pickAction)
            {
                PickActionDescription act = *(static_cast<PickActionDescription *>(ActionManager::actions[actionId]));
                string &toyType = act.toyType;
            }
            if(actType == navigateAction)
            {
                NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
                int &oDestination = act.oDestination;
            }
        __heuristicValue = __heuristicValue < 0 ? 0 : __heuristicValue;
    }

void Collect_toys::SampleModuleExecutionTime(const Collect_toysState& farstate, double rand_num, int actionId, int &__moduleExecutionTime) const
{
    ActionType &actType = ActionManager::actions[actionId]->actionType;
    if(actType == detectAction)
    {
    }
    if(actType == placeAction)
    {
    }
    if(actType == pickAction)
    {
    }
    if(actType == navigateAction)
    {
    }
}

void Collect_toys::ExtrinsicChangesDynamicModel(const Collect_toysState& state, Collect_toysState& state_, double rand_num, int actionId, const int &__moduleExecutionTime,  double &__reward)  const
{
    ActionType &actionType = ActionManager::actions[actionId]->actionType;
    DetectActionDescription* detect = actionType != (detectAction) ? NULL : (static_cast<DetectActionDescription *>(ActionManager::actions[actionId]));
    PlaceActionDescription* place = actionType != (placeAction) ? NULL : (static_cast<PlaceActionDescription *>(ActionManager::actions[actionId]));
    PickActionDescription* pick = actionType != (pickAction) ? NULL : (static_cast<PickActionDescription *>(ActionManager::actions[actionId]));
    NavigateActionDescription* navigate = actionType != (navigateAction) ? NULL : (static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
    state_.observed_reward=false;
    
;
}

void Collect_toys::ModuleDynamicModel(const Collect_toysState &state, const Collect_toysState &state_, Collect_toysState &state__, double rand_num, int actionId, double &__reward, OBS_TYPE &observation, const int &__moduleExecutionTime, const bool &__meetPrecondition) const
{
    std::hash<std::string> hasher;
    ActionType &actType = ActionManager::actions[actionId]->actionType;
    observation = -1;
    observation = default_moduleResponse;
    std::string __moduleResponseStr = "NoStrResponse";
    OBS_TYPE &__moduleResponse = observation;
    if(actType == detectAction)
    {
        __moduleResponseStr = "____________________";
        
/*for(int i=0;
         i < state.tToyObjects.size();
         i++)
{
    int ind = state.tToyObjects[i]->location*4;
        
    float t_rew = state.tToyObjects[i]->reward;
        
    char c_toyReward = t_rew == 40.0 ? 'A' : t_rew == 20.0 ? 'B' : 'C';
        
    __moduleResponseStr [ind] = state.tToyObjects[i]->type[0];
        
    __moduleResponseStr [ind+1] = state.tToyObjects[i]->type[1];
        
    __moduleResponseStr [ind+2] = state.tToyObjects[i]->type[2];
        
    __moduleResponseStr [ind+3] = c_toyReward;
        
}*/

if(state.observed_reward)
{
    for(int i=0;
        
         i < state.tToyObjects.size();
        
         i++)
    {
        int ind = i*5;
        
        
        float t_rew = state.tToyObjects[i]->reward;
        
        
        char c_toyReward = t_rew == 40.0 ? 'A' : t_rew == 20.0 ? 'B' : 'C';
        
        
        __moduleResponseStr [ind] = state.tToyObjects[i]->type[0];
        
        
        __moduleResponseStr [ind+1] = state.tToyObjects[i]->type[1];
        
        
        __moduleResponseStr [ind+2] = state.tToyObjects[i]->type[2];
        
        
        __moduleResponseStr [ind+3] = c_toyReward;
        
        __moduleResponseStr [ind+4] = std::to_string(state.tToyObjects[i]->location)[0];
        
        
    }
}

__reward = state.observed_reward ? 40 : -1.5;
    }
    if(actType == placeAction)
    {
        tToy* toy;
        
bool holding = false;
        
for(int i=0;
         i < state__.tToyObjects.size();
         i++)
{
    if(LArm == (*state__.tToyObjects[i]).location)
    {
        toy = state__.tToyObjects[i];
         
        state__.tToyObjects[i]->location = LChild;
         
        holding=true;
         
        break;
        
    }
}
__moduleResponse = holding ? place_eSuccess : place_eFailed;
        
__reward = !holding ? -1 : (state.robotLocation == LChild ? toy->reward : -1);
    }
    if(actType == pickAction)
    {
        PickActionDescription act = *(static_cast<PickActionDescription *>(ActionManager::actions[actionId]));
        string &toyType = act.toyType;
        state__.pickActionsLeft--;
        
bool success = false;
        
if(__meetPrecondition)
{
    map<string, float>chances = {{"green", 0.5}, {"blue", 0.8},{"black", 1.0}, {"red", 1.0}};
         
    success = AOSUtils::Bernoulli(chances[toyType]);
          
    for(int i=0;
         i < state__.tToyObjects.size();
         i++){
        if(state.robotLocation == state.tToyObjects[i]->location && success)
        {
            state__.tToyObjects[i]->location = LArm;
         break;
        
        }
    }
}
__moduleResponse = success ? pick_eSuccess : pick_eFailed;
        
 __reward = __meetPrecondition ? -1 : -20;
    }
    if(actType == navigateAction)
    {
        NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
        int &oDestination = act.oDestination;
        vector<float> chances{0.5, 0.7, 0.8, 0.85, 0.95};
        
bool success = AOSUtils::Bernoulli(chances[oDestination]);
         
state__.robotLocation = success ? oDestination : state__.robotLocation;
        
__moduleResponse = success ? navigate_eSuccess : navigate_eFailed;
        
__reward = -1;
    }
    if(__moduleResponseStr != "NoStrResponse")
    {
        Collect_toysResponseModuleAndTempEnums responseHash = (Collect_toysResponseModuleAndTempEnums)AOSUtils::get_hash(__moduleResponseStr);
        enum_map_collect_toys::vecResponseEnumToString[responseHash] = __moduleResponseStr;
        enum_map_collect_toys::vecStringToResponseEnum[__moduleResponseStr] = responseHash;
        __moduleResponse = responseHash;
    }
}

bool Collect_toys::ProcessSpecialStates(Collect_toysState &state, double &reward) const
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
             if(state.pickActionsLeft < 0)
{
    __isGoalState =true;
            
}

if(!std::any_of(state.tToyObjects.cbegin(), state.tToyObjects.cend(), [&](tToy* t){ return t->location != LChild;
             }))
{
    __isGoalState =true;
            
}
;
            return std::make_tuple(__reward, __isGoalState, __stopEvaluatingState);
        };
        std::tie(temp_reward, temp_IsGoalState, temp_StopEvaluatingState) = stateFunction();
        state.OneTimeRewardUsed[0] = !temp_StopEvaluatingState;
        reward += temp_reward;
        isFinalState = temp_IsGoalState ? true : isFinalState;
    }
    return isFinalState;
}





std::string Collect_toys::PrintObs(int action, OBS_TYPE obs) const 
{
	return Prints::PrintObs(obs);
}

std::string Collect_toys::PrintStateStr(const State &state) const { return ""; };
}// namespace despot
