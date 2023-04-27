#include "collectValuableToys.h"
#include <despot/core/pomdp.h> 
#include <stdlib.h>
#include <despot/solver/pomcp.h>
#include <sstream>
#include <despot/model_primitives/collectValuableToys/actionManager.h> 
#include <despot/model_primitives/collectValuableToys/enum_map_collectValuableToys.h>  
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

CollectValuableToys CollectValuableToys::gen_model;

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
std::hash<std::string> CollectValuableToys::hasher;
/* ==============================================================================
 *CollectValuableToysBelief class
 * ==============================================================================*/
int CollectValuableToysBelief::num_particles = 5234;
std::string CollectValuableToysBelief::beliefFromDB = "";
int CollectValuableToysBelief::currentInitParticleIndex = -1;

CollectValuableToysBelief::CollectValuableToysBelief(vector<State*> particles, const DSPOMDP* model,
	Belief* prior) :
	ParticleBelief(particles, model, prior),
	collectValuableToys_(static_cast<const CollectValuableToys*>(model)) {
}

	
 	std::string CollectValuableToys::GetActionDescription(int actionId) const
	 {
		 return Prints::PrintActionDescription(ActionManager::actions[actionId]);
	 }

void CollectValuableToysBelief::Update(int actionId, OBS_TYPE obs, map<std::string, std::string> localVariables) {
	history_.Add(actionId, obs);

    ActionType &actType = ActionManager::actions[actionId]->actionType;
    bool skillSuccess3;
    bool skillSuccess2;
    int destination;
    bool skillSuccess;
    string toy_type_name;



    try
    {
        if(actType == navigateAction)
        {
            if(localVariables.find("skillSuccess2") != localVariables.end())
            {
                skillSuccess2 = localVariables["skillSuccess2"] == "true";
            }
            if(localVariables.find("destination") != localVariables.end())
            {
                destination = std::stoi(localVariables["destination"]);
            }
        }
        if(actType == pickAction)
        {
            if(localVariables.find("skillSuccess") != localVariables.end())
            {
                skillSuccess = localVariables["skillSuccess"] == "true";
            }
            if(localVariables.find("toy_type_name") != localVariables.end())
            {
                toy_type_name = localVariables["toy_type_name"];
            }
        }
        if(actType == placeAction)
        {
            if(localVariables.find("skillSuccess3") != localVariables.end())
            {
                skillSuccess3 = localVariables["skillSuccess3"] == "true";
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
		State* particle = collectValuableToys_->Copy(particles_[cur]);
		bool terminal = collectValuableToys_->Step(*particle, Random::RANDOM.NextDouble(),
			actionId, reward, o);
 
		//if (!terminal && o == obs)
        if (o == obs) 
			{
                if(!Globals::IsInternalSimulation())
                {
				CollectValuableToysState &state__ = static_cast<CollectValuableToysState &>(*particles_[cur]);
                CollectValuableToysState &state___ = static_cast<CollectValuableToysState &>(*particle);
                    if(actType == navigateAction)
                    {
                        NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
                        int &oDestination = act.oDestination;
                    }
                    if(actType == pickAction)
                    {
                        PickActionDescription act = *(static_cast<PickActionDescription *>(ActionManager::actions[actionId]));
                        string &toyType = act.toyType;
                    }
                    if(actType == placeAction)
                    {
                    }


                }
				updated.push_back(particle);
		} else {
			collectValuableToys_->Free(particle);
		}

		cur = (cur + 1) % N;

		trials++;
	}

	for (int i = 0; i < particles_.size(); i++)
		collectValuableToys_->Free(particles_[i]);

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
 * CollectValuableToysPOMCPPrior class
 * ==============================================================================*/

class CollectValuableToysPOMCPPrior: public POMCPPrior {
private:
	const CollectValuableToys* collectValuableToys_;

public:
	CollectValuableToysPOMCPPrior(const CollectValuableToys* model) :
		POMCPPrior(model),
		collectValuableToys_(model) {
	}

	void ComputePreference(const State& state) {
		const CollectValuableToysState& collectValuableToys_state = static_cast<const CollectValuableToysState&>(state);
		weighted_preferred_actions_.clear();
        legal_actions_.clear();
		preferred_actions_.clear();
        std::vector<double> weighted_preferred_actions_un_normalized;

        double heuristicValueTotal = 0;
		for (int a = 0; a < 10; a++) {
            weighted_preferred_actions_un_normalized.push_back(0);
			double reward = 0;
			bool meetPrecondition = false; 
			CollectValuableToys::CheckPreconditions(collectValuableToys_state, reward, meetPrecondition, a);
            if(meetPrecondition)
            {
                legal_actions_.push_back(a);
                double __heuristicValue; 
                CollectValuableToys::ComputePreferredActionValue(collectValuableToys_state, __heuristicValue, a);
                heuristicValueTotal += __heuristicValue;
                weighted_preferred_actions_un_normalized[a]=__heuristicValue;
            }
        }

        if(heuristicValueTotal > 0)
        {
            for (int a = 0; a < 10; a++) 
            {
                weighted_preferred_actions_.push_back(weighted_preferred_actions_un_normalized[a] / heuristicValueTotal);
            } 
        }
    }
};

/* ==============================================================================
 * CollectValuableToys class
 * ==============================================================================*/

CollectValuableToys::CollectValuableToys(){
	srand((unsigned int)time(NULL));
}

int CollectValuableToys::NumActions() const {
	return ActionManager::actions.size();
}

double CollectValuableToys::ObsProb(OBS_TYPE obs, const State& state, int actionId) const {
	return 0.9;
}

 


std::default_random_engine CollectValuableToys::generator;


State* CollectValuableToys::CreateStartState(string type) const {
    CollectValuableToysState* startState = memory_pool_.Allocate();
    CollectValuableToysState& state = *startState;
    state.location0=0;
    state.location1=1;
    state.location2=2;
    state.location3=3;
    state.child=4;
    state.robotArm=5;
    state.toy_typeGreen="green";
    state.toy_typeBlue="blue";
    state.toy_typeBlack="black";
    state.toy_typeRed="red";
    state.toy1 = tToy();
    state.toy1.type="green";
    state.toy2 = tToy();
    state.toy2.type="blue";
    state.toy3 = tToy();
    state.toy3.type="black";
    state.toy4 = tToy();
    state.toy4.type="red";
    state.pickActionsLeft=6;
    state.robotLocation=state.child;
    startState->intObjectsForActions["state.location0"] = (state.location0);
    startState->intObjectsForActions["state.location1"] = (state.location1);
    startState->intObjectsForActions["state.location2"] = (state.location2);
    startState->intObjectsForActions["state.location3"] = (state.location3);
    startState->intObjectsForActions["state.child"] = (state.child);
    startState->stringObjectsForActions["state.toy_typeGreen"] = (state.toy_typeGreen);
    startState->stringObjectsForActions["state.toy_typeBlue"] = (state.toy_typeBlue);
    startState->stringObjectsForActions["state.toy_typeBlack"] = (state.toy_typeBlack);
    startState->stringObjectsForActions["state.toy_typeRed"] = (state.toy_typeRed);
    startState->tToyObjects.push_back(&(state.toy1));
    startState->tToyObjects.push_back(&(state.toy2));
    startState->tToyObjects.push_back(&(state.toy3));
    startState->tToyObjects.push_back(&(state.toy4));
    vector<float> weightsLocationGreen{0.1,0.05,0.8,0.05};
    
state.toy1.location = AOSUtils::SampleDiscrete(weightsLocationGreen);
    
vector<float> weightsLocationBlue{0.7,0.1,0.1,0.1};
    
float selectedWeight = weightsLocationBlue[state.toy1.location];
    
for(int i=0;
    i<4;
    i++){weightsLocationBlue[i]+=selectedWeight/3;
    }
weightsLocationBlue[state.toy1.location]=0;
    
state.toy2.location = AOSUtils::SampleDiscrete(weightsLocationBlue);
    
vector<float> weightsLocationBlack{0.25,0.25,0.25,0.25};
    
selectedWeight = weightsLocationBlack[state.toy2.location]+weightsLocationBlack[state.toy1.location];
    
for(int i=0;
    i<4;
    i++){weightsLocationBlack[i]+=selectedWeight/2;
    }
weightsLocationBlack[state.toy1.location]=0;
    weightsLocationBlack[state.toy2.location]=0;
    
state.toy3.location = AOSUtils::SampleDiscrete(weightsLocationBlack);
    
vector<float> weightsLocationRed{1.0,1.0,1.0,1.0};
    
weightsLocationRed[state.toy1.location]=0;
    weightsLocationRed[state.toy2.location]=0;
    weightsLocationRed[state.toy3.location]=0;
    
state.toy4.location = AOSUtils::SampleDiscrete(weightsLocationRed);
    vector<float> rewards{40,20,10,10};
    
vector<float> weightsRewardsGreen{0.8,0.05,0.1,0.05};
    
int selectedRewardIndex1 = AOSUtils::SampleDiscrete(weightsRewardsGreen);
    
state.toy1.reward = rewards[selectedRewardIndex1];
    
vector<float> weightsRewardsBlue{0.1,0.7,0.1,0.1};
    
selectedWeight = weightsRewardsBlue[selectedRewardIndex1];
    
for(int i=0;
    i<4;
    i++){weightsRewardsBlue[i]+=selectedWeight/3;
    }
weightsRewardsBlue[selectedRewardIndex1]=0;
    
int selectedRewardIndex2 = AOSUtils::SampleDiscrete(weightsRewardsBlue);
    
state.toy2.reward = rewards[selectedRewardIndex2];
    
vector<float> weightsRewardsBlack{0.25,0.25,0.25,0.25};
    
selectedWeight = weightsRewardsBlack[selectedRewardIndex1] + weightsRewardsBlack[selectedRewardIndex2];
    
for(int i=0;
    i<4;
    i++){weightsRewardsBlack[i]+=selectedWeight/2;
    }
weightsRewardsBlack[selectedRewardIndex1]=0;
    weightsRewardsBlack[selectedRewardIndex2]=0;
    
int selectedRewardIndex3 = AOSUtils::SampleDiscrete(weightsRewardsBlack);
    
state.toy3.reward = rewards[selectedRewardIndex3];
    
vector<float> weightsRewardsRed{1.0,1.0,1.0,1.0};
    
weightsRewardsRed[selectedRewardIndex1]=0;
    weightsRewardsRed[selectedRewardIndex2]=0;
    weightsRewardsRed[selectedRewardIndex3]=0;
    
state.toy4.reward = rewards[AOSUtils::SampleDiscrete(weightsRewardsRed)];
    if (ActionManager::actions.size() == 0)
    {
        ActionManager::Init(const_cast <CollectValuableToysState*> (startState));
    }
    double r;
    state.__isTermianl = ProcessSpecialStates(state, r);
    return startState;
}




Belief* CollectValuableToys::InitialBelief(const State* start, string type) const {
    if(Globals::config.solveProblemWithClosedPomdpModel)
    {
        POMDP_ClosedModel::closedModel.CreateAndSolveModel();
    }
	int N = CollectValuableToysBelief::num_particles;
	vector<State*> particles(N);
	for (int i = 0; i < N; i++) {
        CollectValuableToysBelief::currentInitParticleIndex = i;
		particles[i] = CreateStartState();
		particles[i]->weight = 1.0 / N;
	}
    CollectValuableToysBelief::currentInitParticleIndex = -1;
	return new CollectValuableToysBelief(particles, this);
}
 

 

POMCPPrior* CollectValuableToys::CreatePOMCPPrior(string name) const { 
		return new CollectValuableToysPOMCPPrior(this);
}

void CollectValuableToys::PrintState(const State& state, ostream& ostr) const {
	const CollectValuableToysState& farstate = static_cast<const CollectValuableToysState&>(state);
	if (ostr)
		ostr << Prints::PrintState(farstate);
}

void CollectValuableToys::PrintObs(const State& state, OBS_TYPE observation,
	ostream& ostr) const {
	const CollectValuableToysState& farstate = static_cast<const CollectValuableToysState&>(state);
	
	ostr << observation <<endl;
}

void CollectValuableToys::PrintBelief(const Belief& belief, ostream& out) const {
	 out << "called PrintBelief(): b printed"<<endl;
		out << endl;
	
}

void CollectValuableToys::PrintAction(int actionId, ostream& out) const {
	out << Prints::PrintActionDescription(ActionManager::actions[actionId]) << endl;
}

State* CollectValuableToys::Allocate(int state_id, double weight) const {
	CollectValuableToysState* state = memory_pool_.Allocate();
	state->state_id = state_id;
	state->weight = weight;
	return state;
}

State* CollectValuableToys::Copy(const State* particle) const {
	CollectValuableToysState* state = memory_pool_.Allocate();
	*state = *static_cast<const CollectValuableToysState*>(particle);
	state->SetAllocated();

    state->tToyObjects[0] = &(state->toy1);
    state->tToyObjects[1] = &(state->toy2);
    state->tToyObjects[2] = &(state->toy3);
    state->tToyObjects[3] = &(state->toy4);


	return state;
}

void CollectValuableToys::Free(State* particle) const {
	memory_pool_.Free(static_cast<CollectValuableToysState*>(particle));
}

int CollectValuableToys::NumActiveParticles() const {
	return memory_pool_.num_allocated();
}

void CollectValuableToys::StepForModel(State& state, int actionId, double& reward,
        OBS_TYPE& observation, int &state_hash, int &next_state_hash, bool& isTerminal, double& precondition_reward, double& specialStateReward) const
    {
        reward = 0;
        CollectValuableToysState &ir_state = static_cast<CollectValuableToysState &>(state);
        state_hash = hasher(Prints::PrintState(ir_state));

        bool meetPrecondition;
        precondition_reward = 0;
        CheckPreconditions(ir_state, reward, meetPrecondition, actionId);
        if(!meetPrecondition)
        {
            precondition_reward = reward;
        }
        
        isTerminal = CollectValuableToys::Step(state, 0.1, actionId, reward,
                   observation);
        ir_state = static_cast<CollectValuableToysState &>(state);

        specialStateReward = 0;
        ProcessSpecialStates(ir_state, specialStateReward);
        reward -= (precondition_reward + specialStateReward);//so that it will not consider the precondition penalty and special states reward

        next_state_hash = hasher(Prints::PrintState(ir_state));
    }
bool CollectValuableToys::Step(State& s_state__, double rand_num, int actionId, double& reward,
	OBS_TYPE& observation) const {
    observation = default_moduleResponse;
    reward = 0;
	Random random(rand_num);
	int __moduleExecutionTime = -1;
	bool meetPrecondition = false;
	double tReward = 0;

	CollectValuableToysState &state__ = static_cast<CollectValuableToysState &>(s_state__);
	 logd << "[CollectValuableToys::Step] Selected Action:" << Prints::PrintActionDescription(ActionManager::actions[actionId]) << "||State"<< Prints::PrintState(state__);
	CheckPreconditions(state__, reward, meetPrecondition, actionId);
	State *s_state = &s_state__;
	CollectValuableToysState &state = static_cast<CollectValuableToysState &>(*s_state);
    SampleModuleExecutionTime(state__, rand_num, actionId, __moduleExecutionTime);

   //no ExtrinsicChangesDynamicModel
    reward += tReward;
    tReward = 0;
    

	State *s_state_ = Copy(&s_state__);
	CollectValuableToysState &state_ = static_cast<CollectValuableToysState &>(*s_state_);

    
	ModuleDynamicModel(state, state_, state__, rand_num, actionId, tReward,
					   observation, __moduleExecutionTime, meetPrecondition);
    
	
    Free(s_state_);
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

void CollectValuableToys::CheckPreconditions(const CollectValuableToysState& state, double &reward, bool &__meetPrecondition, int actionId)
    {
        ActionType &actType = ActionManager::actions[actionId]->actionType;
        __meetPrecondition = true;
            if(actType == navigateAction)
            {
                NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
                int &oDestination = act.oDestination;
                __meetPrecondition = oDestination != state.robotLocation;
                if(!__meetPrecondition) reward += -1;
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
                 i++){
    if(state.robotArm == state.tToyObjects[i]->location){ holding = true;
                 break;
                }
    if(state.robotLocation == state.tToyObjects[i]->location && state.tToyObjects[i]->type == toyType) typeMatch=true;
                
}
__meetPrecondition = !holding && typeMatch && state.robotLocation != state.child;
                if(!__meetPrecondition) reward += 0;
            }
            if(actType == placeAction)
            {
                __meetPrecondition = false;
                
for(int i=0;
                 i < state.tToyObjects.size();
                 i++){
    if(state.robotArm == (*state.tToyObjects[i]).location) {__meetPrecondition = true;
                 break;
                }};
                if(!__meetPrecondition) reward += 0;
            }
    }

void CollectValuableToys::ComputePreferredActionValue(const CollectValuableToysState& state, double &__heuristicValue, int actionId)
    {
        __heuristicValue = 0;
        ActionType &actType = ActionManager::actions[actionId]->actionType;
            if(actType == navigateAction)
            {
                NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
                int &oDestination = act.oDestination;
                __heuristicValue=0;
                
bool holding = false;
                
float max_reward = 0;
                
tToy* toy=NULL;
                
bool hasToyAtLocation = false;
                
for(int i=0;
                 i < state.tToyObjects.size();
                 i++){
   tToy _toy = *state.tToyObjects[i];
                
    if(state.robotArm == _toy.location) holding = true;
                
    if(_toy.location == state.robotLocation && _toy.location != state.child) hasToyAtLocation = true;
                
    if(state.child != _toy.location && _toy.reward > max_reward) max_reward = _toy.reward;
                
    if(oDestination == _toy.location) toy= state.tToyObjects[i];
                
}
__heuristicValue = holding || oDestination == state.child ? (holding && oDestination == state.child ? 100 : 0)
 :(hasToyAtLocation ? 0 : (toy != NULL && toy->reward == max_reward ? 100 : 0));
            }
            if(actType == pickAction)
            {
                PickActionDescription act = *(static_cast<PickActionDescription *>(ActionManager::actions[actionId]));
                string &toyType = act.toyType;
                __heuristicValue=0;
                
bool holding = false;
                
bool typeMatch = false;
                
for(int i=0;
                 i < state.tToyObjects.size();
                 i++){
    if(state.robotArm == state.tToyObjects[i]->location){ holding = true;
                 break;
                }
    if(state.robotLocation == state.tToyObjects[i]->location && state.tToyObjects[i]->type == toyType) typeMatch=true;
                
}
__heuristicValue = !holding && typeMatch ? 100 : 0;
            }
            if(actType == placeAction)
            {
                for(int i=0;
                 i < state.tToyObjects.size();
                 i++){
    if(state.robotArm == (*state.tToyObjects[i]).location && state.robotLocation == state.child){ __heuristicValue=100;
                 break;
                }};
            }
        __heuristicValue = __heuristicValue < 0 ? 0 : __heuristicValue;
    }

void CollectValuableToys::SampleModuleExecutionTime(const CollectValuableToysState& farstate, double rand_num, int actionId, int &__moduleExecutionTime) const
{
    ActionType &actType = ActionManager::actions[actionId]->actionType;
    if(actType == navigateAction)
    {
    }
    if(actType == pickAction)
    {
    }
    if(actType == placeAction)
    {
    }
}

void CollectValuableToys::ExtrinsicChangesDynamicModel(const CollectValuableToysState& state, CollectValuableToysState& state_, double rand_num, int actionId, const int &__moduleExecutionTime,  double &__reward)  const
{
    ActionType &actionType = ActionManager::actions[actionId]->actionType;
    NavigateActionDescription* navigate = actionType != (navigateAction) ? NULL : (static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
    PickActionDescription* pick = actionType != (pickAction) ? NULL : (static_cast<PickActionDescription *>(ActionManager::actions[actionId]));
    PlaceActionDescription* place = actionType != (placeAction) ? NULL : (static_cast<PlaceActionDescription *>(ActionManager::actions[actionId]));
}

void CollectValuableToys::ModuleDynamicModel(const CollectValuableToysState &state, const CollectValuableToysState &state_, CollectValuableToysState &state__, double rand_num, int actionId, double &__reward, OBS_TYPE &observation, const int &__moduleExecutionTime, const bool &__meetPrecondition) const
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
        int &oDestination = act.oDestination;
        state__.robotLocation = oDestination;
        
__moduleResponse = navigate_eSuccess;
        
__reward = __meetPrecondition ? -3 : -4;
    }
    if(actType == pickAction)
    {
        PickActionDescription act = *(static_cast<PickActionDescription *>(ActionManager::actions[actionId]));
        string &toyType = act.toyType;
        bool success =  __meetPrecondition;
        
state__.pickActionsLeft--;
        
if(success){
for(int i=0;
         i < state__.tToyObjects.size();
         i++){
    if(state.robotLocation == state.tToyObjects[i]->location){ state__.tToyObjects[i]->location = state.robotArm;
         break;
        }}};
        __moduleResponse = success ? pick_eSuccess : pick_eFailed;
        __reward = success ? -1 : -2;
    }
    if(actType == placeAction)
    {
        tToy* toy;
        
bool holding = false;
        
for(int i=0;
         i < state__.tToyObjects.size();
         i++){
    if(state.robotArm == (*state__.tToyObjects[i]).location){ toy = state__.tToyObjects[i];
         state__.tToyObjects[i]->location = state.child;
         holding=true;
         break;
        }};
        __moduleResponse = holding ? place_eSuccess : place_eFailed;
        
__reward = !holding ? -3 : (state.robotLocation == state.child ? toy->reward : -1);
    }
    if(__moduleResponseStr != "NoStrResponse")
    {
        CollectValuableToysResponseModuleAndTempEnums responseHash = (CollectValuableToysResponseModuleAndTempEnums)hasher(__moduleResponseStr);
        enum_map_collectValuableToys::vecResponseEnumToString[responseHash] = __moduleResponseStr;
        enum_map_collectValuableToys::vecStringToResponseEnum[__moduleResponseStr] = responseHash;
        __moduleResponse = responseHash;
    }
}

bool CollectValuableToys::ProcessSpecialStates(CollectValuableToysState &state, double &reward) const
{
    float temp_reward = 0;
    bool temp_IsGoalState = false;
    bool temp_StopEvaluatingState = false;
    bool isFinalState = false;
    if(state.OneTimeRewardUsed[0])
    {
        if (state.pickActionsLeft == 0 && !std::any_of(state.tToyObjects.cbegin(), state.tToyObjects.cend(), [&](tToy* t){ return t->location == state.robotArm; }))
        {
            reward += 0;
            isFinalState = true;
        }
    }
    if(state.OneTimeRewardUsed[1])
    {
        if (state.pickActionsLeft < 0)
        {
            reward += 0;
            isFinalState = true;
        }
    }
    if(state.OneTimeRewardUsed[2])
    {
        if (!std::any_of(state.tToyObjects.cbegin(), state.tToyObjects.cend(), [&](tToy* t){ return t->location != state.child; }))
        {
            reward += 0;
            isFinalState = true;
        }
    }
    return isFinalState;
}





std::string CollectValuableToys::PrintObs(int action, OBS_TYPE obs) const 
{
	return Prints::PrintObs(obs);
}

std::string CollectValuableToys::PrintStateStr(const State &state) const { return ""; };
}// namespace despot
