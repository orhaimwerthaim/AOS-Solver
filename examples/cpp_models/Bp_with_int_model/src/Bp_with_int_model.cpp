#include "Bp_with_int_model.h"
#include <despot/core/pomdp.h> 
#include <stdlib.h>
#include <despot/solver/pomcp.h>
#include <sstream>
#include <despot/model_primitives/Bp_with_int_model/actionManager.h> 
#include <despot/model_primitives/Bp_with_int_model/enum_map_Bp_with_int_model.h> 
#include <despot/model_primitives/Bp_with_int_model/state.h> 
#include <algorithm>
#include <cmath> 
#include <despot/util/mongoDB_Bridge.h>
#include <functional> //for std::hash
#include <set>
#include <unistd.h>
#include <iomanip>
#include <float.h>

using namespace std;

namespace despot {


bool AOSUtils::Bernoulli(double p)
{
	/* generate secret number between 1 and 100: */
    srand((unsigned int)time(NULL));
	int randInt = rand() % 100 + 1;
	return (p * 100) >= randInt;
}
std::hash<std::string> Bp_with_int_model::hasher;
/* ==============================================================================
 *Bp_with_int_modelBelief class
 * ==============================================================================*/
int Bp_with_int_modelBelief::num_particles = 5000;
std::string Bp_with_int_modelBelief::beliefFromDB = "";
int Bp_with_int_modelBelief::currentInitParticleIndex = -1;

Bp_with_int_modelBelief::Bp_with_int_modelBelief(vector<State*> particles, const DSPOMDP* model,
	Belief* prior) :
	ParticleBelief(particles, model, prior),
	Bp_with_int_model_(static_cast<const Bp_with_int_model*>(model)) {
}

	
 	std::string Bp_with_int_model::GetActionDescription(int actionId) const
	 {
		 return Prints::PrintActionDescription(ActionManager::actions[actionId]);
	 }

//void Bp_with_int_modelBelief::Update(int actionId, OBS_TYPE obs, std::map<std::string,bool> updates) {
void Bp_with_int_modelBelief::Update(int actionId, OBS_TYPE obs) {
	history_.Add(actionId, obs);

	vector<State*> updated;
	double reward;
	OBS_TYPE o;
	int cur = 0, N = particles_.size(), trials = 0;
	while (updated.size() < num_particles && trials < 10 * num_particles) {
		State* particle = Bp_with_int_model_->Copy(particles_[cur]);
		bool terminal = Bp_with_int_model_->Step(*particle, Random::RANDOM.NextDouble(),
			actionId, reward, o);
 
		if (!terminal && o == obs) 
			{
				Bp_with_int_modelState &Bp_with_int_model_particle = static_cast<Bp_with_int_modelState &>(*particle);
				//if(!Globals::IsInternalSimulation() && updates.size() > 0)
				//{
				//	Bp_with_int_modelState::SetAnyValueLinks(&Bp_with_int_model_particle);
				//	map<std::string, bool>::iterator it;
				//	for (it = updates.begin(); it != updates.end(); it++)
				//	{
				//		*(Bp_with_int_model_particle.anyValueUpdateDic[it->first]) = it->second; 
				//	} 
				//}
				updated.push_back(particle);
		} else {
			Bp_with_int_model_->Free(particle);
		}

		cur = (cur + 1) % N;

		trials++;
	}

	for (int i = 0; i < particles_.size(); i++)
		Bp_with_int_model_->Free(particles_[i]);

	particles_ = updated;

	for (int i = 0; i < particles_.size(); i++)
		particles_[i]->weight = 1.0 / particles_.size();
 
}

/* ==============================================================================
 * Bp_with_int_modelPOMCPPrior class
 * ==============================================================================*/

class Bp_with_int_modelPOMCPPrior: public POMCPPrior {
private:
	const Bp_with_int_model* Bp_with_int_model_;

public:
	Bp_with_int_modelPOMCPPrior(const Bp_with_int_model* model) :
		POMCPPrior(model),
		Bp_with_int_model_(model) {
	}

	void ComputePreference(const State& state) {
		const Bp_with_int_modelState& Bp_with_int_model_state = static_cast<const Bp_with_int_modelState&>(state);
		weighted_preferred_actions_.clear();
        legal_actions_.clear();
		preferred_actions_.clear();
        std::vector<double> weighted_preferred_actions_un_normalized;

        double heuristicValueTotal = 0;
		for (int a = 0; a < 12; a++) {
            weighted_preferred_actions_un_normalized.push_back(0);
			double reward = 0;
			bool meetPrecondition = false; 
			Bp_with_int_model::CheckPreconditions(Bp_with_int_model_state, reward, meetPrecondition, a);
            if(meetPrecondition)
            {
                legal_actions_.push_back(a);
                double __heuristicValue; 
                Bp_with_int_model::ComputePreferredActionValue(Bp_with_int_model_state, __heuristicValue, a);
                heuristicValueTotal += __heuristicValue;
                weighted_preferred_actions_un_normalized[a]=__heuristicValue;
            }
        }

        if(heuristicValueTotal > 0)
        {
            for (int a = 0; a < 12; a++) 
            {
                weighted_preferred_actions_.push_back(weighted_preferred_actions_un_normalized[a] / heuristicValueTotal);
            } 
        }
    }
};

/* ==============================================================================
 * Bp_with_int_model class
 * ==============================================================================*/

Bp_with_int_model::Bp_with_int_model(){
	
}

int Bp_with_int_model::NumActions() const {
	return ActionManager::actions.size();
}

double Bp_with_int_model::ObsProb(OBS_TYPE obs, const State& state, int actionId) const {
	return 0.9;
}

 


std::default_random_engine Bp_with_int_model::generator;


State* Bp_with_int_model::CreateStartState(string type) const {
    Bp_with_int_modelState* startState = memory_pool_.Allocate();
    Bp_with_int_modelState& state = *startState;
    startState->tPushTypeObjects.push_back(SingleAgentPush);
    startState->tPushTypeObjects.push_back(JointPush);
    startState->tDirectionObjects.push_back(Up);
    startState->tDirectionObjects.push_back(Down);
    startState->tDirectionObjects.push_back(Left);
    startState->tDirectionObjects.push_back(Right);
    startState->tDirectionObjects.push_back(None);
    state.bTwoLocGoal = tLocation();
    state.bTwoLocGoal.x=3;
    state.bTwoLocGoal.y=3;
    state.bOneLocGoal = tLocation();
    state.bOneLocGoal.x=1;
    state.bOneLocGoal.y=1;
    state.agentOneLoc = tLocation();
    state.agentOneLoc.x=0;
    state.agentOneLoc.y=1;
    state.agentTwoLoc = tLocation();
    state.agentTwoLoc.x=1;
    state.agentTwoLoc.y=0;
    state.bOneLoc = tLocation();
    state.bOneLoc.x=0;
    state.bOneLoc.y=0;
    state.bTwoLoc = tLocation();
    state.bTwoLoc.x=0;
    state.bTwoLoc.y=0;
    state.isAgentOneTurn = true;
    state.ParamUp = Up;;
    state.ParamDown = Down;;
    state.ParamLeft = Left;;
    state.ParamRight = Right;;
    state.ParamSingleAgentPush = SingleAgentPush;;
    state.ParamJointPush = JointPush;;
    state.JointPushDirection = None;;
    state.MaxGridx = 3;;
    state.MaxGridy = 3;;
    startState->tLocationObjects.push_back(&(state.bTwoLocGoal));
    startState->tLocationObjects.push_back(&(state.bOneLocGoal));
    startState->tLocationObjects.push_back(&(state.agentOneLoc));
    startState->tLocationObjects.push_back(&(state.agentTwoLoc));
    startState->tLocationObjects.push_back(&(state.bOneLoc));
    startState->tLocationObjects.push_back(&(state.bTwoLoc));
    startState->tDirectionObjectsForActions["state.ParamUp"] = (state.ParamUp);
    startState->tDirectionObjectsForActions["state.ParamDown"] = (state.ParamDown);
    startState->tDirectionObjectsForActions["state.ParamLeft"] = (state.ParamLeft);
    startState->tDirectionObjectsForActions["state.ParamRight"] = (state.ParamRight);
    startState->tPushTypeObjectsForActions["state.ParamSingleAgentPush"] = (state.ParamSingleAgentPush);
    startState->tPushTypeObjectsForActions["state.ParamJointPush"] = (state.ParamJointPush);
    if (ActionManager::actions.size() == 0)
    {
        ActionManager::Init(const_cast <Bp_with_int_modelState*> (startState));
    }
    return startState;
}



struct state_tran
{		
    bool hasSpecialStateReward = false;
    double specialStateReward;
    map<int, double> actionPreconditionReward;
    map<std::pair<int,int>,double> actionPrevStateReward;
    int state;
    map<int,int> total_samplesFromStateByAction;
    map<int,int> total_samplesToStateByAction;
    map<std::pair<int,int>, int> actionObservationCount;//when arriving to this state by action and observation
    map<std::pair<int,int>, int> actionNextStateCount;//when arriving to next state by action
    
    map<std::pair<int,int>, double> actionObservationProb;
    map<std::pair<int,int>, double> actionNextStateProb;
    bool isTerminalState = false;

    void setAsTerminal()
    {
        isTerminalState = true;
        actionNextStateProb.clear();
        //actionPrevStateReward.insert({std::pair<int,int>{_action, state}, 0});//inner loop 0 rewads for terminal state
    }

    void addObservationTransitionRewardData(int _state, int _nextState, int _action, int _observation, double _reward, bool _isNextStateTerminal, double precondition_reward, double _specialStateReward)
    {
        if(_nextState == state)
        {
            if(_specialStateReward != 0)
            {
                hasSpecialStateReward = true;
                specialStateReward = _specialStateReward;
            }
            addObservationAsNextStateSample(_nextState, _observation, _action);
            if(_isNextStateTerminal)
            {
                setAsTerminal();
            }
            actionPrevStateReward.insert({std::pair<int,int>{_action, _state}, _reward});
        }

        if(state == _state)
        {
            if(precondition_reward != 0)
            {
                actionPreconditionReward[_action]=precondition_reward;
            }
        }

        if(state == _state && !isTerminalState)
        {
            addNextStateTransition(_state, _nextState, _action);
        }
    } 

    void addObservationAsNextStateSample(int _nextState, int _obs, int _action)
    { 
        auto actIt = total_samplesToStateByAction.find(_action);
        total_samplesToStateByAction[_action] = (actIt == total_samplesToStateByAction.end()) ? 1 : total_samplesToStateByAction[_action] + 1;
        std::pair<int, int> actObsP{_action, _obs};
        auto it = actionObservationCount.find(actObsP);
          state_tran *st = NULL;
          actionObservationCount[actObsP] = (it == actionObservationCount.end()) ? 1 : actionObservationCount[actObsP] + 1;    
    }

    void addNextStateTransition(int _state, int next_state, int _action)
      { 
          auto actIt = total_samplesFromStateByAction.find(_action);
        total_samplesFromStateByAction[_action] = (actIt == total_samplesFromStateByAction.end()) ? 1 : total_samplesFromStateByAction[_action] + 1;
 
          std::pair<int, int> actNextStateP{_action, next_state};
          auto it = actionNextStateCount.find(actNextStateP);
          state_tran *st = NULL;
          actionNextStateCount[actNextStateP] = (it == actionNextStateCount.end()) ? 1 : actionNextStateCount[actNextStateP] + 1;      
      }

      void calcModelBySamples()
      {
            for (auto const& x : actionNextStateCount)
            {
                actionNextStateProb.insert({x.first, ((double)x.second/total_samplesFromStateByAction[x.first.first])});
            }
            for (auto const& x : actionObservationCount)
            {
                actionObservationProb.insert({x.first, ((double)x.second/total_samplesToStateByAction[x.first.first])});
            }
      }
};
	 
struct model_data 
  {
      int initialBStateSamplesCount = 0;
      
      map<int, int> initialBStateParticle;
      map<int, double> stateInitialStateSampleProb;
      map<int, state_tran> statesModel; 
      map<int, std::string> statesToPomdpFileStates;

      void addInitialBStateSample(int _state)
      {
          initialBStateSamplesCount++;
          initialBStateParticle[_state] = initialBStateParticle.find(_state) == initialBStateParticle.end() ? 1 : initialBStateParticle[_state] + 1;
      }
      
      void addSample(int state, int nextState, int action, int observation, double reward, bool isNextStateTerminal, double precondition_reward, double specialStateReward)
      {
          
          state_tran *st = getStateModel(state);
          state_tran *n_st = getStateModel(nextState);
          n_st->isTerminalState = isNextStateTerminal;
          
          st->addObservationTransitionRewardData(state, nextState, action, observation, reward, isNextStateTerminal, precondition_reward, specialStateReward);
          n_st->addObservationTransitionRewardData(state, nextState, action, observation, reward, isNextStateTerminal, precondition_reward, specialStateReward);
      }
      state_tran * getStateModel(int state)
      {
          auto it = statesModel.find(state);
          state_tran *st = NULL;
          if (it == statesModel.end())
          {
              st = new state_tran;
              st->state = state;
              statesModel.insert({state, *st}); 
          }
          else
          {
              st = &(it->second);
          }
          return st;
      }
      void calcModelBySamples()
      {
          for (auto & x : statesModel)
            {
                x.second.calcModelBySamples();
            }
      }
  };

void Bp_with_int_model::CreateAndSolveModel() const
{
    memory_pool_.DeleteAll();
    int horizon = Globals::config.search_depth;
    int numOfSamplesForEachActionFromState = Globals::config.numOfSamplesPerActionStateWhenLearningTheModel;
    model_data modelD;
    std::set<int> states;
    std::set<int> observations;
    std::map<int, State*> statesToProcessNext;
    std::map<int, State*> statesToProcessCurr;
    std::map<int, std::string> actionsToDesc;
    std::map<int, std::string> observationsToDesc; 
    
    for (int i = 0; i < ActionManager::actions.size();i++)
    {
        std::string temp = Prints::PrintActionDescription(ActionManager::actions[i]);
        std::replace( temp.begin(), temp.end(), ':', '_');
        std::replace( temp.begin(), temp.end(), ',', '_');
        std::replace( temp.begin(), temp.end(), ' ', '_');
        temp.insert(0, "a_");
        actionsToDesc.insert({i, temp});
    }
    for (int i = 0; i < 1000; i++)
    {
        State *state = CreateStartState();
        Bp_with_int_modelState &ir_state = static_cast<Bp_with_int_modelState &>(*state);
        int hash = hasher(Prints::PrintState(ir_state));
        if (states.insert(hash).second)
        {
            statesToProcessCurr.insert({hash, state});
        }
        else
        {
                   Free(state);
        }
        modelD.addInitialBStateSample(hash);
    }
    bool goalstateFound = false;
    for (int i = 0; i < horizon;i++)
    {
        for (auto & stateP : statesToProcessCurr)
            {
                for (int action = 0; action < Bp_with_int_model::NumActions(); action++)
                {
                    for (int sampleCount = 0; sampleCount < numOfSamplesForEachActionFromState; sampleCount++)
                    {                
                        double reward=0;
                        OBS_TYPE obs;
                        int state_hash;
                        int nextStateHash;
                        bool isNextStateTerminal;
                        State *next_state = Copy(stateP.second);
                        double precondition_reward;
                        double specialStateReward;
                        StepForModel(*next_state, action, reward, obs, state_hash, nextStateHash, isNextStateTerminal, precondition_reward, specialStateReward);

                        if(isNextStateTerminal && reward > 0 && !goalstateFound)
                        {
                            goalstateFound = true;
                            horizon = (Globals::config.limitClosedModelHorizon_stepsAfterGoalDetection < 0) ? horizon : i + Globals::config.limitClosedModelHorizon_stepsAfterGoalDetection; 
                        }

                        modelD.addSample(state_hash, nextStateHash, action, obs, reward, isNextStateTerminal, precondition_reward, specialStateReward);
                        if(observations.insert(obs).second);

                        auto it = modelD.statesModel.find(nextStateHash);
                        bool skip = false;
                        if (it != modelD.statesModel.end())
                        {
                            state_tran *st = &(it->second);
                            skip = st->isTerminalState; 
                        }
                        if(!skip)
                        {
                            if(states.insert(nextStateHash).second)
                            {
                                statesToProcessNext.insert({nextStateHash, next_state});
                            }
                            else
                            {
                                Free(next_state);
                            }
                        }
                        else
                        {
                            Free(next_state);
                        }
                    }
                }
            }
            statesToProcessCurr.clear();
            statesToProcessCurr.insert(statesToProcessNext.begin(), statesToProcessNext.end());
            statesToProcessNext.clear();
    }
    
    modelD.calcModelBySamples();
  
        char tmp[256];
                getcwd(tmp, 256);
                std::string workinDirPath(tmp);
                workinDirPath = workinDirPath.substr(0, workinDirPath.find("build"));
        std::string fpath(workinDirPath);
        fpath.append(Globals::config.pomdpFilePath);
        std::ofstream fs;
        remove(fpath.c_str());
        fs.open(fpath, std::ios_base::app); //std::ios_base::app
        fs << "discount: " << Globals::config.discount << endl;
        fs << endl;
        fs << "values: reward" << endl;
        fs << endl;
        fs << "states:";
        int count = 0;
        for (auto &stateN : states)
        {
            std::string stateName= std::to_string(count++).insert(0, "s_");
            modelD.statesToPomdpFileStates[stateN] = stateName;
            fs << " " << stateName;
        }
        fs << endl;
        fs << endl;
        fs << "actions: ";
        for (auto & actD : actionsToDesc)
        {
            fs << actD.second << " ";
        }
        fs << endl;
        fs << endl;
        
        fs << "observations: ";
        count = 0;
        for (int  obs : observations)
        {
            std::string s = "o" + std::to_string(count++) + "_" + Prints::PrintObs(0, obs);
                            //s.insert(0, "o_");
                            observationsToDesc.insert({obs, s});
            fs << s << " ";
        }
        std::string invalidObsS = "o" + std::to_string(count++) + "_invalidObs";
            // for (auto &obsD : observationsToDesc)
            // {
            //     fs << obsD.second << " ";
            // }
        fs << invalidObsS << " ";
        fs << endl;
        fs << endl; 		
        fs << endl;
        fs << "start:" << endl;
        for (auto & stateN : modelD.statesToPomdpFileStates)
        {  
            double prob = (double)modelD.initialBStateParticle[stateN.first] / (double)modelD.initialBStateSamplesCount;
            std::stringstream stream;
            stream << std::fixed << std::setprecision(1) << prob;
            std::string s = stream.str();
            
            fs << " " << s;
        }
        fs << endl; 		
        fs << endl;


        map<int, set<int>> actionStatesWithoutAnyTran;
        for (int act = 0; act < ActionManager::actions.size();act++)
        {
            actionStatesWithoutAnyTran[act] = set<int>{states};
        }
        for (auto &stateT : modelD.statesModel)
        { 
            for (auto &actNStateProb : stateT.second.actionNextStateProb)
            {
                actionStatesWithoutAnyTran[actNStateProb.first.first].erase(stateT.first); 
                fs << "T: " << actionsToDesc[actNStateProb.first.first] << " : " << modelD.statesToPomdpFileStates[stateT.first] << " : " << modelD.statesToPomdpFileStates[actNStateProb.first.second] << " " << std::to_string(actNStateProb.second) << endl;
            } 
        }
 
        for(auto &actionStateWithoutAnyTranision: actionStatesWithoutAnyTran)
                {
                    for (auto &state:actionStateWithoutAnyTranision.second)
                    {
                        fs << "T: " << actionsToDesc[actionStateWithoutAnyTranision.first] << " : " << modelD.statesToPomdpFileStates[state] << " : " << modelD.statesToPomdpFileStates[state] << " 1.0" << endl;
                    }  
                }
        
        fs << endl;
        fs << endl;
        fs << endl;


        //check actions that have the same observation from any state
        map<int,std::string> actionWithSingleObservation;
        for (int act = 0; act < ActionManager::actions.size();act++)
        {
            actionWithSingleObservation[act] = invalidObsS;
        }
        for(auto & stateT : modelD.statesModel)
        {
            
            for (auto &actObsProb : stateT.second.actionObservationProb)
            {
                //change observation from default value to first seen value
                if(actionWithSingleObservation.find(actObsProb.first.first) != actionWithSingleObservation.end())
                {
                    actionWithSingleObservation[actObsProb.first.first] = actionWithSingleObservation[actObsProb.first.first] == invalidObsS ? observationsToDesc[actObsProb.first.second] : actionWithSingleObservation[actObsProb.first.first];
                    if(actionWithSingleObservation[actObsProb.first.first] != observationsToDesc[actObsProb.first.second])
                    {
                        actionWithSingleObservation.erase(actObsProb.first.first);
                    }
                }
            }
        }
        for(auto &actSingleObs : actionWithSingleObservation)
        {
            fs << "O: " << actionsToDesc[actSingleObs.first] << " : * : " << actSingleObs.second << " 1.0" << endl;
        }

        //to make sure that all the stat-action pairs have observations defined.
        map<int,std::set<int>> allStatePerAction;
        for (int act = 0; act < ActionManager::actions.size();act++)
        {
            if(actionWithSingleObservation.find(act) == actionWithSingleObservation.end())
            {
                allStatePerAction[act] = set<int>{states};
            }
        }
        for(auto & stateT : modelD.statesModel)
        {
            
            for (auto &actObsProb : stateT.second.actionObservationProb)
            {
                if(actionWithSingleObservation.find(actObsProb.first.first) == actionWithSingleObservation.end())
                {
                    allStatePerAction[actObsProb.first.first].erase(stateT.first);
                    fs << "O: " << actionsToDesc[actObsProb.first.first] << " : " << modelD.statesToPomdpFileStates[stateT.first] << " : " << observationsToDesc[actObsProb.first.second] << " " << std::to_string(actObsProb.second) << endl;
                }

            }
        }
        //adding invalid observations to fill missing ones
        for(auto &missingObss: allStatePerAction)
            {
                for(auto &StateWithMissingObs: missingObss.second)
                {
                    fs << "O: " << actionsToDesc[missingObss.first] << " : " << modelD.statesToPomdpFileStates[StateWithMissingObs] << " : " << invalidObsS << " 1.0" << endl;
                }    
            }
        fs << endl;
        fs << endl;
        fs << endl;

        map<int, double> actionSingleReward;//check if action has a cost not independent of next state (we dont check the precondition reward here)
        for (int action = 0; action < NumActions(); action++)
        {
            actionSingleReward[action] = DBL_MAX;
        }

        for (auto &stateR : modelD.statesModel)
        {
            for(auto & actPrevStateReward : stateR.second.actionPrevStateReward)
            { 
                if(actionSingleReward.find(actPrevStateReward.first.first) != actionSingleReward.end())
                {
                    actionSingleReward[actPrevStateReward.first.first] = (actionSingleReward[actPrevStateReward.first.first] == DBL_MAX) ? actPrevStateReward.second : actionSingleReward[actPrevStateReward.first.first];
                    if (actionSingleReward[actPrevStateReward.first.first] != actPrevStateReward.second)
                        actionSingleReward.erase(actPrevStateReward.first.first);
                }
            }
        }
        for (auto &actSingleReward : actionSingleReward)//rewards for action with same cost for each state
        {
            fs << "R: " << actionsToDesc[actSingleReward.first] << " : * : * : * " << std::to_string(actSingleReward.second) << endl;
        }
        for (auto &stateR : modelD.statesModel)//rewards for spesial states
        {
            if(stateR.second.hasSpecialStateReward)
            {
                 fs << "R: * : * : " + modelD.statesToPomdpFileStates[stateR.first] + " : * " << std::to_string(stateR.second.specialStateReward) << endl;
            }
        }
        for (auto &stateR : modelD.statesModel)
        {
            //adding precondition penalty
            for(auto & actPreconditionReward : stateR.second.actionPreconditionReward)
            {  
                fs << "R: " << actionsToDesc[actPreconditionReward.first] << " : " << modelD.statesToPomdpFileStates[stateR.first]  << " : * : * " << std::to_string(actPreconditionReward.second) << endl;
            }
        }

        for (auto &stateR : modelD.statesModel)
        {
            //adding action cost or reward when is depends on preconditions (regardless of preconditions)
            for(auto & actPrevStateReward : stateR.second.actionPrevStateReward)
            {  
                if(actionSingleReward.find(actPrevStateReward.first.first) == actionSingleReward.end())
                {
                    fs << "R: " << actionsToDesc[actPrevStateReward.first.first] << " : " << modelD.statesToPomdpFileStates[actPrevStateReward.first.second]  << " : " << modelD.statesToPomdpFileStates[stateR.first] << " : * " << std::to_string(actPrevStateReward.second) << endl;
                }
            }
        }
        
        
        //Prints::PrintActionDescription(ActionManager::actions[actionId])
        //Prints::PrintActionDescription()
        fs << "" << endl;
        fs << "" << endl;
        fs << "" << endl;
  fs.close();
//after writing the .pomdp file. compile sarsop and solve the problem.
  std::string cmd = "cd ";
  cmd.append(workinDirPath);
  cmd.append("sarsop/src ; make ; ./pomdpsol ");
  cmd.append(workinDirPath);
  cmd.append("sarsop/examples/POMDP/auto_generate.pomdp");
  if(Globals::config.sarsopTimeLimitInSeconds > 0)
  {
      cmd.append(" --timeout ");
      cmd.append(std::to_string(Globals::config.sarsopTimeLimitInSeconds));
  }//   ./pomdpsol /home/or/Projects/sarsop/examples/POMDP/auto_generate.pomdp
  cmd.append(" ; ./polgraph --policy-file out.policy --policy-graph autoGen.dot ");
  cmd.append(workinDirPath);
  cmd.append("sarsop/examples/POMDP/auto_generate.pomdp");
  
  //cmd.append(" ; dot -Tpdf autoGen.dot -o outfile_autoGen.pdf"); //uncomment if you want to generate a pdf graph
  
  std::string policyFilePath(workinDirPath);
  policyFilePath.append("sarsop/src/out.policy");
  remove(policyFilePath.c_str());
  
  std::string dotFilePath(workinDirPath);
  dotFilePath.append("sarsop/src/autoGen.dot");
  remove(dotFilePath.c_str());

  std::string graphFilePath(workinDirPath);
  graphFilePath.append("sarsop/src/outfile_autoGen.pdf");
  remove(graphFilePath.c_str());

  system(cmd.c_str());
}

Belief* Bp_with_int_model::InitialBelief(const State* start, string type) const {
    if(Globals::config.generatePOMDP_modelFile)
    {
        CreateAndSolveModel();
    }
	int N = Bp_with_int_modelBelief::num_particles;
	vector<State*> particles(N);
	for (int i = 0; i < N; i++) {
        Bp_with_int_modelBelief::currentInitParticleIndex = i;
		particles[i] = CreateStartState();
		particles[i]->weight = 1.0 / N;
	}
    Bp_with_int_modelBelief::currentInitParticleIndex = -1;
	return new Bp_with_int_modelBelief(particles, this);
}
 

 

POMCPPrior* Bp_with_int_model::CreatePOMCPPrior(string name) const { 
		return new Bp_with_int_modelPOMCPPrior(this);
}

std::string Bp_with_int_model::GetCellDesc(int x, int y, const Bp_with_int_modelState& state) const
{
	std::string res = "";
	
	if(state.agentOneLoc.x == x && state.agentOneLoc.y == y)
	{
		res += (state.isAgentOneTurn ? "A1*" : "A1");
		if(state.JointPushDirection != None)
		{
			res += state.JointPushDirection == Up ? "^J" : state.JointPushDirection == Down ? "vJ"
													  : state.JointPushDirection == Left   ? "<J"
																						   : ">J";
		}
	}
	
	if(state.agentTwoLoc.x == x && state.agentTwoLoc.y == y)
	{
		res += res.length() > 0 ? "," : "";
		res += (state.isAgentOneTurn ? "A2" : "A2*");
	} 
	if(state.bOneLoc.x == x && state.bOneLoc.y == y)
	{
		res += res.length() > 0 ? ",B1" : "B1";
	}
	if(state.bTwoLoc.x == x && state.bTwoLoc.y == y)
	{
		res += res.length() > 0 ? ",B2" : "B2";
	}
	int gapsNeeded = 11 - res.length();
	std::string start(int(ceil(gapsNeeded / 2)), ' ');
	std::string end(int(floor(gapsNeeded / 2)), ' ');

	res = start + res + end;
	return res;
}

 


void Bp_with_int_model::PrintState(const State& _state, ostream& ostr) const {
	const Bp_with_int_modelState& state = static_cast<const Bp_with_int_modelState&>(_state);
	try
	{
	if (ostr)
		{
		std::string line((12*(state.MaxGridx+1)), '-');
		for(int y=state.MaxGridx; y >=0; y--)
		{
			ostr << line << endl;
			for (int x = 0; x <= state.MaxGridx; x++)
			{
				ostr << "|" << GetCellDesc(x, y, state);
			}
			ostr << "|" << endl;
		}
		ostr << line << endl;
		} 
	}
	catch(const std::exception& e)
	{

	}
}

void Bp_with_int_model::PrintObs(const State& state, OBS_TYPE observation,
	ostream& ostr) const {
	const Bp_with_int_modelState& farstate = static_cast<const Bp_with_int_modelState&>(state);
	
	ostr << observation <<endl;
}

void Bp_with_int_model::PrintBelief(const Belief& belief, ostream& out) const {
	 out << "called PrintBelief(): b printed"<<endl;
		out << endl;
	
}

void Bp_with_int_model::PrintAction(int actionId, ostream& out) const {
	out << Prints::PrintActionDescription(ActionManager::actions[actionId]) << endl;
}

State* Bp_with_int_model::Allocate(int state_id, double weight) const {
	Bp_with_int_modelState* state = memory_pool_.Allocate();
	state->state_id = state_id;
	state->weight = weight;
	return state;
}

State* Bp_with_int_model::Copy(const State* particle) const {
	Bp_with_int_modelState* state = memory_pool_.Allocate();
	*state = *static_cast<const Bp_with_int_modelState*>(particle);
	state->SetAllocated();

    state->tLocationObjects[0] = &(state->bTwoLocGoal);
    state->tLocationObjects[1] = &(state->bOneLocGoal);
    state->tLocationObjects[2] = &(state->agentOneLoc);
    state->tLocationObjects[3] = &(state->agentTwoLoc);
    state->tLocationObjects[4] = &(state->bOneLoc);
    state->tLocationObjects[5] = &(state->bTwoLoc);


	return state;
}

void Bp_with_int_model::Free(State* particle) const {
	memory_pool_.Free(static_cast<Bp_with_int_modelState*>(particle));
}

int Bp_with_int_model::NumActiveParticles() const {
	return memory_pool_.num_allocated();
}

void Bp_with_int_model::StepForModel(State& state, int actionId, double& reward,
        OBS_TYPE& observation, int &state_hash, int &next_state_hash, bool& isTerminal, double& precondition_reward, double& specialStateReward) const
    {
        reward = 0;
        Bp_with_int_modelState &ir_state = static_cast<Bp_with_int_modelState &>(state);
        state_hash = hasher(Prints::PrintState(ir_state));

        bool meetPrecondition;
        precondition_reward = 0;
        CheckPreconditions(ir_state, reward, meetPrecondition, actionId);
        if(!meetPrecondition)
        {
            precondition_reward = reward;
        }
        
        isTerminal = Bp_with_int_model::Step(state, 0.1, actionId, reward,
                   observation);
        ir_state = static_cast<Bp_with_int_modelState &>(state);

        specialStateReward = 0;
        ProcessSpecialStates(ir_state, specialStateReward);
        reward -= (precondition_reward + specialStateReward);//so that it will not consider the precondition penalty and special states reward

        next_state_hash = hasher(Prints::PrintState(ir_state));
    }

bool Bp_with_int_model::Step(State& s_state__, double rand_num, int actionId, double& reward,
	OBS_TYPE& observation) const {
    reward = 0;
	bool isNextStateFinal = false;
	Random random(rand_num);
	int __moduleExecutionTime = -1;
	bool meetPrecondition = false;
	
	Bp_with_int_modelState &state__ = static_cast<Bp_with_int_modelState &>(s_state__);
	 logd << "[Bp_with_int_model::Step] Selected Action:" << Prints::PrintActionDescription(ActionManager::actions[actionId]) << "||State"<< Prints::PrintState(state__);
	CheckPreconditions(state__, reward, meetPrecondition, actionId);
	 
	State *s_state = Copy(&s_state__);
	Bp_with_int_modelState &state = static_cast<Bp_with_int_modelState &>(*s_state);

	
	SampleModuleExecutionTime(state__, rand_num, actionId, __moduleExecutionTime);

	ExtrinsicChangesDynamicModel(state, state__, rand_num, actionId, __moduleExecutionTime);

	State *s_state_ = Copy(&s_state__);
	Bp_with_int_modelState &state_ = static_cast<Bp_with_int_modelState &>(*s_state_);

    double tReward = 0;
	ModuleDynamicModel(state, state_, state__, rand_num, actionId, tReward,
					   observation, __moduleExecutionTime, meetPrecondition);
	reward += tReward;

	Free(s_state);
	Free(s_state_);
	bool finalState = ProcessSpecialStates(state__, reward);

    if (!meetPrecondition)
	{
		//__moduleExecutionTime = 0;
		//observation = illegalActionObs;
		//return false;
	}
	return finalState;
}

void Bp_with_int_model::CheckPreconditions(const Bp_with_int_modelState& state, double &reward, bool &__meetPrecondition, int actionId)
    {
        ActionType &actType = ActionManager::actions[actionId]->actionType;
        __meetPrecondition = true;
            if(actType == pushAction)
            {
                PushActionDescription act = *(static_cast<PushActionDescription *>(ActionManager::actions[actionId]));
                tDirection &oDirection = act.oDirection;
                tPushType &oIsJointPush = act.oIsJointPush;
                if(oIsJointPush==JointPush&&!state.isAgentOneTurn&&oDirection!=state.JointPushDirection)__meetPrecondition=false;
                if(oIsJointPush==JointPush&&state.agentOneLoc!=state.bTwoLoc)__meetPrecondition=false;
                if(oIsJointPush==SingleAgentPush&&((state.isAgentOneTurn&&state.agentOneLoc!=state.bOneLoc)||(!state.isAgentOneTurn&&state.agentTwoLoc!=state.bOneLoc)))__meetPrecondition=false;
                if(oIsJointPush==JointPush&&state.agentOneLoc!=state.agentTwoLoc)__meetPrecondition=false;
                if(state.isAgentOneTurn&&state.agentOneLoc.y>=state.MaxGridy&&oDirection==Up)__meetPrecondition=false;
                if(state.isAgentOneTurn&&state.agentOneLoc.y<=0&&oDirection==Down)__meetPrecondition=false;
                if(state.isAgentOneTurn&&state.agentOneLoc.x<=0&&oDirection==Left)__meetPrecondition=false;
                if(state.isAgentOneTurn&&state.agentOneLoc.x>=state.MaxGridx&&oDirection==Right)__meetPrecondition=false;
                if(!state.isAgentOneTurn&&state.agentTwoLoc.y>=state.MaxGridy&&oDirection==Up)__meetPrecondition=false;
                if(!state.isAgentOneTurn&&state.agentTwoLoc.y<=0&&oDirection==Down)__meetPrecondition=false;
                if(!state.isAgentOneTurn&&state.agentTwoLoc.x<=0&&oDirection==Left)__meetPrecondition=false;
                if(!state.isAgentOneTurn&&state.agentTwoLoc.x>=state.MaxGridx&&oDirection==Right)__meetPrecondition=false;
                if(!__meetPrecondition) reward += 0;
            }
            if(actType == navigateAction)
            {
                NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
                tDirection &oDirection = act.oDirection;
                if(state.isAgentOneTurn&&state.agentOneLoc.y>=state.MaxGridy&&oDirection==Up)__meetPrecondition=false;
                if(state.isAgentOneTurn&&state.agentOneLoc.y<=0&&oDirection==Down)__meetPrecondition=false;
                if(state.isAgentOneTurn&&state.agentOneLoc.x<=0&&oDirection==Left)__meetPrecondition=false;
                if(state.isAgentOneTurn&&state.agentOneLoc.x>=state.MaxGridx&&oDirection==Right)__meetPrecondition=false;
                if(!state.isAgentOneTurn&&state.agentTwoLoc.y>=state.MaxGridy&&oDirection==Up)__meetPrecondition=false;
                if(!state.isAgentOneTurn&&state.agentTwoLoc.y<=0&&oDirection==Down)__meetPrecondition=false;
                if(!state.isAgentOneTurn&&state.agentTwoLoc.x<=0&&oDirection==Left)__meetPrecondition=false;
                if(!state.isAgentOneTurn&&state.agentTwoLoc.x>=state.MaxGridx&&oDirection==Right)__meetPrecondition=false;
                if(!__meetPrecondition) reward += 0;
            }
    }

void Bp_with_int_model::ComputePreferredActionValue(const Bp_with_int_modelState& state, double &__heuristicValue, int actionId)
    {
        __heuristicValue = 0;
        ActionType &actType = ActionManager::actions[actionId]->actionType;
            if(actType == pushAction)
            {
                PushActionDescription act = *(static_cast<PushActionDescription *>(ActionManager::actions[actionId]));
                tDirection &oDirection = act.oDirection;
                tPushType &oIsJointPush = act.oIsJointPush;
                if(oIsJointPush==JointPush&&!state.isAgentOneTurn&&oDirection==state.JointPushDirection)__heuristicValue=100;
                if(__heuristicValue==0)__heuristicValue=1;
            }
            if(actType == navigateAction)
            {
                NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
                tDirection &oDirection = act.oDirection;
                int agentX;
                agentX=state.isAgentOneTurn?state.agentOneLoc.x:state.agentTwoLoc.x;
                int agentY;
                agentY=state.isAgentOneTurn?state.agentOneLoc.y:state.agentTwoLoc.y;
                int nextAgentX;
                nextAgentX=agentX+(oDirection==Left?-1:(oDirection==Right?1:0));
                int nextAgentY;
                nextAgentY=agentY+(oDirection==Down?-1:(oDirection==Up?1:0));
                float bOneManhattanDistance;
                bOneManhattanDistance=std::abs(nextAgentX-state.bOneLoc.x)+std::abs(nextAgentY-state.bOneLoc.y);
                float bTwoManhattanDistance;
                bTwoManhattanDistance=(state.bTwoLoc.x==state.bTwoLocGoal.x&&state.bTwoLoc.y==state.bTwoLocGoal.y)?0:std::abs(nextAgentX-state.bTwoLoc.x)+std::abs(nextAgentY-state.bTwoLoc.y);
                bTwoManhattanDistance=bTwoManhattanDistance==0?0.1:bTwoManhattanDistance;
                bOneManhattanDistance=bOneManhattanDistance==0?0.1:bOneManhattanDistance;
                if((state.bOneLoc.x==state.bOneLocGoal.x&&state.bOneLoc.y==state.bOneLocGoal.y)||(state.bOneLoc.x==agentX&&state.bOneLoc.y==agentY))bOneManhattanDistance=-1;
                if((state.bTwoLoc.x==state.bTwoLocGoal.x&&state.bTwoLoc.y==state.bTwoLocGoal.y)||(state.bTwoLoc.x==agentX&&state.bTwoLoc.y==agentY))bTwoManhattanDistance=-1;
                __heuristicValue=std::max(1/bTwoManhattanDistance,1/bOneManhattanDistance);
            }
        __heuristicValue = __heuristicValue < 0 ? 0 : __heuristicValue;
    }

void Bp_with_int_model::SampleModuleExecutionTime(const Bp_with_int_modelState& farstate, double rand_num, int actionId, int &__moduleExecutionTime) const
{
    ActionType &actType = ActionManager::actions[actionId]->actionType;
    if(actType == pushAction)
    {
    }
    if(actType == navigateAction)
    {
    }
}

void Bp_with_int_model::ExtrinsicChangesDynamicModel(const Bp_with_int_modelState& state, Bp_with_int_modelState& state_, double rand_num, int actionId, const int &__moduleExecutionTime)  const
{
    ActionType &actType = ActionManager::actions[actionId]->actionType;
        state_.isAgentOneTurn=!state.isAgentOneTurn;
}

void Bp_with_int_model::ModuleDynamicModel(const Bp_with_int_modelState &state, const Bp_with_int_modelState &state_, Bp_with_int_modelState &state__, double rand_num, int actionId, double &__reward, OBS_TYPE &observation, const int &__moduleExecutionTime, const bool &__meetPrecondition) const
{
    std::hash<std::string> hasher;
    ActionType &actType = ActionManager::actions[actionId]->actionType;
    observation = -1;
    int startObs = observation;
    std::string __moduleResponseStr = "NoStrResponse";
    OBS_TYPE &__moduleResponse = observation;
    if(actType == pushAction)
    {
        PushActionDescription act = *(static_cast<PushActionDescription *>(ActionManager::actions[actionId]));
        tDirection &oDirection = act.oDirection;
        tPushType &oIsJointPush = act.oIsJointPush;
        if(oDirection==Down&&__meetPrecondition){if(oIsJointPush==SingleAgentPush)state__.bOneLoc.y--;
        else if(!state.isAgentOneTurn)state__.bTwoLoc.y--;
        };
        if(oDirection==Up&&__meetPrecondition){if(oIsJointPush==SingleAgentPush)state__.bOneLoc.y++;
        else if(!state.isAgentOneTurn)state__.bTwoLoc.y++;
        };
        if(oDirection==Left&&__meetPrecondition){if(oIsJointPush==SingleAgentPush)state__.bOneLoc.x--;
        else if(!state.isAgentOneTurn)state__.bTwoLoc.x--;
        };
        if(oDirection==Right&&__meetPrecondition){if(oIsJointPush==SingleAgentPush)state__.bOneLoc.x++;
        else if(!state.isAgentOneTurn)state__.bTwoLoc.x++;
        };
        if(__meetPrecondition&&oIsJointPush==JointPush){if(state.isAgentOneTurn)state__.JointPushDirection=oDirection;
        };
        if(!(state.isAgentOneTurn&&oIsJointPush==JointPush))state__.JointPushDirection=None;
        __moduleResponse=push_eSuccess;
        __reward=-1;
    }
    if(actType == navigateAction)
    {
        NavigateActionDescription act = *(static_cast<NavigateActionDescription *>(ActionManager::actions[actionId]));
        tDirection &oDirection = act.oDirection;
        if(oDirection==Down&&__meetPrecondition){if(state.isAgentOneTurn)state__.agentOneLoc.y--;
        else state__.agentTwoLoc.y--;
        };
        if(oDirection==Up&&__meetPrecondition){if(state.isAgentOneTurn)state__.agentOneLoc.y++;
        else state__.agentTwoLoc.y++;
        };
        if(oDirection==Left&&__meetPrecondition){if(state.isAgentOneTurn)state__.agentOneLoc.x--;
        else state__.agentTwoLoc.x--;
        };
        if(oDirection==Right&&__meetPrecondition){if(state.isAgentOneTurn)state__.agentOneLoc.x++;
        else state__.agentTwoLoc.x++;
        };
        state__.JointPushDirection=None;
        __moduleResponse=navigate_eSuccess;
        __reward=-1;
    }
    if(__moduleResponseStr != "NoStrResponse")
    {
        Bp_with_int_modelResponseModuleAndTempEnums responseHash = (Bp_with_int_modelResponseModuleAndTempEnums)hasher(__moduleResponseStr);
        enum_map_Bp_with_int_model::vecResponseEnumToString[responseHash] = __moduleResponseStr;
        enum_map_Bp_with_int_model::vecStringToResponseEnum[__moduleResponseStr] = responseHash;
        __moduleResponse = responseHash;
    }
    if(startObs == __moduleResponse)
    {
    stringstream ss;
    ss << "Observation/__moduleResponse Not initialized!!! on action:" << Prints::PrintActionDescription(ActionManager::actions[actionId]) << endl;
    loge << ss.str() << endl;
    throw 1;
    }
}

bool Bp_with_int_model::ProcessSpecialStates(Bp_with_int_modelState &state, double &reward) const
{
    bool isFinalState = false;
    if(state.OneTimeRewardUsed[0])
    {
        if (state.bOneLoc.x==state.bOneLocGoal.x && state.bOneLoc.y==state.bOneLocGoal.y && state.bTwoLoc.x==state.bTwoLocGoal.x && state.bTwoLoc.y==state.bTwoLocGoal.y)
        {
            reward += 8300;
            isFinalState = true;
        }
    }
    return isFinalState;
}





std::string Bp_with_int_model::PrintObs(int action, OBS_TYPE obs) const 
{
	return Prints::PrintObs(action, obs);
}

std::string Bp_with_int_model::PrintStateStr(const State &state) const { return ""; };
}// namespace despot
