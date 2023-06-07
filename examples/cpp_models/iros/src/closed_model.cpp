

#include <filesystem>
namespace fs = std::filesystem;
#include "closed_model.h" 
namespace despot {
      bool AlphaVectorPolicy::isInit = false;



POMDP_ClosedModelState::POMDP_ClosedModelState()
{
    
    POMDP_ClosedModelState::state;
    POMDP_ClosedModelState::total_samplesFromStateByAction;
    POMDP_ClosedModelState::total_samplesToStateByAction;
    POMDP_ClosedModelState::actionObservationCount;//when arriving to this state by action and observation
    POMDP_ClosedModelState::actionNextStateCount;//when arriving to next state by action
    
    //reward model
    POMDP_ClosedModelState::hasSpecialStateReward = false;
    POMDP_ClosedModelState::specialStateReward;//reward when arriving to this state
    POMDP_ClosedModelState::actionPreconditionReward; //map<int, double>; penalty, when starting an action from this state
    POMDP_ClosedModelState::actionPrevStateReward;//map<std::pair<int,int>,double> ;the action cost: starting from prevState executing action, arriving to this state

    //observation model
    POMDP_ClosedModelState::actionObservationProb; //map<std::pair<int,int>, double>; probability to receive an observation after reaching this state using the action
    
    //transition model
    POMDP_ClosedModelState::actionNextStateProb;   //map<std::pair<int,int>, double>; <action,prevState>, probability: probability to reach state from prevstate and action. 
    POMDP_ClosedModelState::isTerminalState = false;
}

#pragma region POMDP_ClosedModelState
    void POMDP_ClosedModelState::addObservationTransitionRewardData(int _state, int _nextState, int _action, int _observation, double _reward, bool _isNextStateTerminal, double precondition_reward, double _specialStateReward)
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

    void POMDP_ClosedModelState::addObservationAsNextStateSample(int _nextState, int _obs, int _action)
    { 
        auto actIt = total_samplesToStateByAction.find(_action);
        total_samplesToStateByAction[_action] = (actIt == total_samplesToStateByAction.end()) ? 1 : total_samplesToStateByAction[_action] + 1;
        std::pair<int, int> actObsP{_action, _obs};
        auto it = actionObservationCount.find(actObsP);
          POMDP_ClosedModelState *st = NULL;
          actionObservationCount[actObsP] = (it == actionObservationCount.end()) ? 1 : actionObservationCount[actObsP] + 1;    
    }

    void POMDP_ClosedModelState::addNextStateTransition(int _state, int next_state, int _action)
      { 
          auto actIt = total_samplesFromStateByAction.find(_action);
        total_samplesFromStateByAction[_action] = (actIt == total_samplesFromStateByAction.end()) ? 1 : total_samplesFromStateByAction[_action] + 1;
 
          std::pair<int, int> actNextStateP{_action, next_state};
          auto it = actionNextStateCount.find(actNextStateP);
          POMDP_ClosedModelState *st = NULL;
          actionNextStateCount[actNextStateP] = (it == actionNextStateCount.end()) ? 1 : actionNextStateCount[actNextStateP] + 1;      
      }

      void POMDP_ClosedModelState::calcModelBySamples()
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


void POMDP_ClosedModelState::setAsTerminal()
    {
        isTerminalState = true;
        actionNextStateProb.clear();
        //actionPrevStateReward.insert({std::pair<int,int>{_action, state}, 0});//inner loop 0 rewads for terminal state
    }
#pragma endregion


#pragma region POMDP_ClosedModel 
POMDP_ClosedModel::POMDP_ClosedModel()
{
    int initialBStateSamplesCount = 0;
    initialBStateParticle;//map<int, int> 
    stateInitialStateSampleProb;//map<int, double> 
    statesModel;//map<int, POMDP_ClosedModelState> 
    statesToPomdpFileStates;//map<int, std::string>
}

void POMDP_ClosedModel::addInitialBStateSample(int _state)
      {
          initialBStateSamplesCount++;
          initialBStateParticle[_state] = initialBStateParticle.find(_state) == initialBStateParticle.end() ? 1 : initialBStateParticle[_state] + 1;
      }

void POMDP_ClosedModel::addSample(int state, int nextState, int action, int observation, double reward, bool isNextStateTerminal, double precondition_reward, double specialStateReward)
      {
          
          POMDP_ClosedModelState *st = getStateModel(state);
          POMDP_ClosedModelState *n_st = getStateModel(nextState);
          n_st->isTerminalState = isNextStateTerminal;
          
          st->addObservationTransitionRewardData(state, nextState, action, observation, reward, isNextStateTerminal, precondition_reward, specialStateReward);
          if(state!=nextState)
          {
            n_st->addObservationTransitionRewardData(state, nextState, action, observation, reward, isNextStateTerminal, precondition_reward, specialStateReward);
          }
      }

      POMDP_ClosedModelState * POMDP_ClosedModel::getStateModel(int state)
      {
          auto it = statesModel.find(state);
          POMDP_ClosedModelState *st = NULL;
          if (it == statesModel.end())
          {
              st = new POMDP_ClosedModelState;
              st->state = state;
              statesModel.insert({state, *st}); 
          }
          else
          {
              st = &(it->second);
          }
          return st;
      }
      
      void POMDP_ClosedModel::calcModelBySamples()
      {
          for (auto & x : statesModel)
            {
                x.second.calcModelBySamples();
            }
      }


void POMDP_ClosedModel::GenerateModelFile(std::set<int> states, std::map<int, std::string> actionsToDesc,
   std::set<int> observations, std::map<int, std::string> observationsToDesc)
{
     map<int, int> stateToPolicyIndex;
     map<std::string, int> actionToPolicyIndex;
     vector<double> initialBeliefState;
     //map<std::string, map<int,map<int, double>>> obsActNState_ObservationModel;//map<observation, map<action,map<nextState, Probability>>>
 
        char tmp[256];
        getcwd(tmp, 256);
        std::string workinDirPath(tmp);
        workinDirPath = workinDirPath.substr(0, workinDirPath.find("build"));
        std::string fpath(workinDirPath);
        fpath.append(Globals::config.pomdpFilePath);
        
        std::string stateMapFile(workinDirPath);
        stateMapFile.append("sarsop/src/state_to_hash.txt");
        std::ofstream fs_states;
        //remove(stateMapFile.c_str());
        fs_states.open(stateMapFile, std::ios_base::app);

        std::string metadata_path(workinDirPath);
        metadata_path.append("sarsop/src/policy_metadata.txt");
        std::ofstream fs_metadata;
        fs_metadata.open(metadata_path, std::ios_base::app);
        fs_metadata << "domain_hash " << Globals::config.domainHash << " search_depth " << Globals::config.search_depth;
        fs_metadata.close();
        std::ofstream fs;
        remove(fpath.c_str());
        fs.open(fpath, std::ios_base::app); //std::ios_base::app
        fs << "discount: " << Globals::config.discount << endl;
        fs << endl;
        fs << "values: reward" << endl;
        fs << endl;
        fs << "states:";
        int count = 0;
        bool first = true;
        for (auto &stateN : states)
        { 
            std::string stateName= std::to_string(count++).insert(0, "s_");
            POMDP_ClosedModel::closedModel.statesToPomdpFileStates[stateN] = stateName;
            fs << " " << stateName;

            fs_states << (first ? "" : ",") << stateN << ":" << stateName;
            first = false;
            stateToPolicyIndex[stateN] = count - 1;
        }
        fs_states.close();
        fs << endl;
        fs << endl;
        fs << "actions: ";
        for (auto & actD : actionsToDesc)
        {
            actionToPolicyIndex[actD.second] = actD.first;
            fs << actD.second << " ";
        }
        fs << endl;
        fs << endl;
        
        fs << "observations: ";
        count = 0;
        for (int  obs : observations)
        {
            //std::string s = "o" + std::to_string(count++) + "_" + Prints::PrintObs(0, obs);
            std::string s = Prints::PrintObs(obs); 
                            observationsToDesc.insert({obs, s});
            fs << s << " ";
        }
        //std::string invalidObsS = "o" + std::to_string(count++) + "_invalidObs";
        std::string invalidObsS = "invalidObs";
            // for (auto &obsD : observationsToDesc)
            // {
            //     fs << obsD.second << " ";
            // }
        fs << invalidObsS << " ";
        fs << endl;
        fs << endl; 		
        fs << endl;
        fs << "start:" << endl;
        double total=0;
        int precision = 4;
        for (auto & stateN : POMDP_ClosedModel::closedModel.statesToPomdpFileStates)
        {  
            double prob = (double)POMDP_ClosedModel::closedModel.initialBStateParticle[stateN.first] / (double)POMDP_ClosedModel::closedModel.initialBStateSamplesCount;
            prob = (floor(prob*pow(10,precision)))/pow(10,precision);
            total+=prob;
            initialBeliefState.push_back(prob);
        }

        if(total != 1.0)
        {
            int maxElementIndex = std::max_element(initialBeliefState.begin(),initialBeliefState.end()) - initialBeliefState.begin();
            initialBeliefState[maxElementIndex]+=1.0-total;
        }

        for(double prob:initialBeliefState)
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(precision) << prob;
            std::string s = stream.str();
            
            fs << " " << s;
        }
        fs << endl; 		
        fs << endl;

        //ClosedModelPolicy::loadInitialBeliefStateFromVector(initialBeliefState);

        map<int, set<int>> actionStatesWithoutAnyTran;
        for (int act = 0; act < ActionManager::actions.size();act++)
        {
            actionStatesWithoutAnyTran[act] = set<int>{states};
        }
        for (auto &stateT : POMDP_ClosedModel::closedModel.statesModel)
        { 
            for (auto &actNStateProb : stateT.second.actionNextStateProb)
            {
                actionStatesWithoutAnyTran[actNStateProb.first.first].erase(stateT.first); 
                fs << "T: " << actionsToDesc[actNStateProb.first.first] << " : " << POMDP_ClosedModel::closedModel.statesToPomdpFileStates[stateT.first] << " : " << POMDP_ClosedModel::closedModel.statesToPomdpFileStates[actNStateProb.first.second] << " " << std::to_string(actNStateProb.second) << endl;

                int actionPol = actionToPolicyIndex[actionsToDesc[actNStateProb.first.first]];
                int nextStatePol = stateToPolicyIndex[actNStateProb.first.second];
                int preStatePol = stateToPolicyIndex[stateT.first];
                ClosedModelPolicy::nextStateActionPrevState_TransitionModel[nextStatePol][actionPol][preStatePol] = actNStateProb.second;
            } 
        }
 
        for(auto &actionStateWithoutAnyTranision: actionStatesWithoutAnyTran)
                {
                    for (auto &state:actionStateWithoutAnyTranision.second)
                    {
                        fs << "T: " << actionsToDesc[actionStateWithoutAnyTranision.first] << " : " << POMDP_ClosedModel::closedModel.statesToPomdpFileStates[state] << " : " << POMDP_ClosedModel::closedModel.statesToPomdpFileStates[state] << " 1.0" << endl;
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
        for(auto & stateT : POMDP_ClosedModel::closedModel.statesModel)
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
            std::string actDesc = actionsToDesc[actSingleObs.first];
            fs << "O: " << actDesc << " : * : " << actSingleObs.second << " 1.0" << endl;


            int actionPol = actionToPolicyIndex[actDesc];
            std::string observation = actSingleObs.second;
            ClosedModelPolicy::obsActNState_ObservationModel[observation][actionPol][STATE_WILD_CARD] = 1.0;
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
        for(auto & stateT : POMDP_ClosedModel::closedModel.statesModel)
        {
            
            for (auto &actObsProb : stateT.second.actionObservationProb)
            {
                if(actionWithSingleObservation.find(actObsProb.first.first) == actionWithSingleObservation.end())
                {
                    allStatePerAction[actObsProb.first.first].erase(stateT.first);
                    fs << "O: " << actionsToDesc[actObsProb.first.first] << " : " << POMDP_ClosedModel::closedModel.statesToPomdpFileStates[stateT.first] << " : " << observationsToDesc[actObsProb.first.second] << " " << std::to_string(actObsProb.second) << endl;

                    int actionPol = actionToPolicyIndex[actionsToDesc[actObsProb.first.first]];
                    ClosedModelPolicy::obsActNState_ObservationModel[observationsToDesc[actObsProb.first.second]][actionPol][stateToPolicyIndex[stateT.first]] = actObsProb.second;
                }

            }
        }
        //adding invalid observations to fill missing ones
        for(auto &missingObss: allStatePerAction)
            {
                for(auto &StateWithMissingObs: missingObss.second)
                {
                    fs << "O: " << actionsToDesc[missingObss.first] << " : " << POMDP_ClosedModel::closedModel.statesToPomdpFileStates[StateWithMissingObs] << " : " << invalidObsS << " 1.0" << endl;
                
                    int actionPol = actionToPolicyIndex[actionsToDesc[missingObss.first]];

                    ClosedModelPolicy::obsActNState_ObservationModel[invalidObsS][actionPol][stateToPolicyIndex[StateWithMissingObs]] = 1.0; 
                }    
            }
        fs << endl;
        fs << endl;
        fs << endl;

        map<int, double> actionSingleReward;//check if action has a cost not independent of next state (we dont check the precondition reward here)
        for (int action = 0; action < ActionManager::actions.size(); action++)
        {
            actionSingleReward[action] = DBL_MAX;
        }

        for (auto &stateR : POMDP_ClosedModel::closedModel.statesModel)
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
        for (auto &stateR : POMDP_ClosedModel::closedModel.statesModel)//rewards for spesial states
        {
            if(stateR.second.hasSpecialStateReward)
            {
                 fs << "R: * : * : " + POMDP_ClosedModel::closedModel.statesToPomdpFileStates[stateR.first] + " : * " << std::to_string(stateR.second.specialStateReward) << endl;
            }
        }
        for (auto &stateR : POMDP_ClosedModel::closedModel.statesModel)
        {
            //adding precondition penalty
            for(auto & actPreconditionReward : stateR.second.actionPreconditionReward)
            {  
                fs << "R: " << actionsToDesc[actPreconditionReward.first] << " : " << POMDP_ClosedModel::closedModel.statesToPomdpFileStates[stateR.first]  << " : * : * " << std::to_string(actPreconditionReward.second) << endl;
            }
        }

        for (auto &stateR : POMDP_ClosedModel::closedModel.statesModel)
        {
            //adding action cost or reward when is depends on preconditions (regardless of preconditions)
            for(auto & actPrevStateReward : stateR.second.actionPrevStateReward)
            {  
                if(actionSingleReward.find(actPrevStateReward.first.first) == actionSingleReward.end())
                {
                    fs << "R: " << actionsToDesc[actPrevStateReward.first.first] << " : " << POMDP_ClosedModel::closedModel.statesToPomdpFileStates[actPrevStateReward.first.second]  << " : " << POMDP_ClosedModel::closedModel.statesToPomdpFileStates[stateR.first] << " : * " << std::to_string(actPrevStateReward.second) << endl;
                }
            }
        }
        
        
        //Prints::PrintActionDescription(ActionManager::actions[actionId])
        //Prints::PrintActionDescription()
        fs << "" << endl;
        fs << "" << endl;
        fs << "" << endl;
  fs.close();
}

void POMDP_ClosedModel::solveModel()
{
    char tmp[256];
    getcwd(tmp, 256);
    std::string workinDirPath(tmp);
    workinDirPath = workinDirPath.substr(0, workinDirPath.find("build"));

    std::string cmd = "cd ";
  cmd.append(workinDirPath);
  cmd.append("sarsop/src ; make ; ./pomdpsol ");
  cmd.append("--policy-interval 10 ");
  cmd.append(workinDirPath);
  cmd.append("sarsop/examples/POMDP/auto_generate.pomdp");
//   if(Globals::config.sarsopTimeLimitInSeconds > 0)
//   {
//       cmd.append(" --timeout ");
//       cmd.append(std::to_string(Globals::config.sarsopTimeLimitInSeconds));
//   }//   ./pomdpsol /home/or/Projects/sarsop/examples/POMDP/auto_generate.pomdp

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


POMDP_ClosedModel POMDP_ClosedModel::closedModel;


//Iros
void POMDP_ClosedModel::CreateAndSolveModel() const
{ 
    string best_policy_fname("out.policy");
		char tmp[256];
		getcwd(tmp, 256);
		std::string workingDirPath(tmp);
		workingDirPath = workingDirPath.substr(0, workingDirPath.find("build"));
        std::string pathToPoliciesDir(workingDirPath);
		pathToPoliciesDir.append("sarsop/src");
		vector<string> files_to_delete;
		for (const auto & entry : fs::directory_iterator(pathToPoliciesDir))
		{
			string ends_with(".policy");
			string file_p(entry.path().u8string());
			string fname(file_p);
            fname = fname.substr(pathToPoliciesDir.size()+1);
            
            if(file_p.size() >= ends_with.size() &&
                file_p.compare(file_p.size() - ends_with.size(), ends_with.size(), ends_with) == 0)
            {
                files_to_delete.push_back(file_p);
            }
            if(fname == "state_to_hash.txt" || fname == "policy_metadata.txt")
            {
                files_to_delete.push_back(file_p);
            }
		}
        for(string del : files_to_delete)
        {
            const char* ca = del.c_str();
            std::remove(ca);
        }

    int horizon = Globals::config.search_depth;
    int numOfSamplesForEachActionFromState = Globals::config.numOfSamplesPerActionStateWhenLearningTheModel;

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
        State *state = Iros::gen_model.CreateStartState();
        int hash = State::GetStateHash(*state);
        if (states.insert(hash).second)
        {
            statesToProcessCurr.insert({hash, state});
        }
        else
        {
                   Iros::gen_model.Free(state);
        }
        POMDP_ClosedModel::closedModel.addInitialBStateSample(hash);
    }
    bool goalstateFound = false;
    for (int i = 0; i < horizon;i++)
    {
        for (auto & stateP : statesToProcessCurr)
            {
                for (int action = 0; action < Iros::gen_model.NumActions(); action++)
                {
                    for (int sampleCount = 0; sampleCount < numOfSamplesForEachActionFromState; sampleCount++)
                    {
                        double reward=0;
                        OBS_TYPE obs;
                        int state_hash;
                        int nextStateHash;
                        bool isNextStateTerminal;
                        State *next_state = Iros::gen_model.Copy(stateP.second);
                        double precondition_reward;
                        double specialStateReward;
                        Iros::gen_model.StepForModel(*next_state, action, reward, obs, state_hash, nextStateHash, isNextStateTerminal, precondition_reward, specialStateReward);

                        if(isNextStateTerminal && specialStateReward > 0 && !goalstateFound)
                        {
                            goalstateFound = true;
                            horizon = (Globals::config.limitClosedModelHorizon_stepsAfterGoalDetection < 0) ? horizon : i + Globals::config.limitClosedModelHorizon_stepsAfterGoalDetection; 
                        }

                        POMDP_ClosedModel::closedModel.addSample(state_hash, nextStateHash, action, obs, reward, isNextStateTerminal, precondition_reward, specialStateReward);
                        if(observations.insert(obs).second);

                        auto it = POMDP_ClosedModel::closedModel.statesModel.find(nextStateHash);
                        bool skip = false;
                        if (it != POMDP_ClosedModel::closedModel.statesModel.end())
                        {
                            POMDP_ClosedModelState *st = &(it->second);
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
                                Iros::gen_model.Free(next_state);
                            }
                        }
                        else
                        {
                            Iros::gen_model.Free(next_state);
                        }
                    }
                }
            }
            statesToProcessCurr.clear();
            statesToProcessCurr.insert(statesToProcessNext.begin(), statesToProcessNext.end());
            statesToProcessNext.clear();
    }
    
    POMDP_ClosedModel::closedModel.calcModelBySamples();
    POMDP_ClosedModel::closedModel.GenerateModelFile(states, actionsToDesc,
 observations, observationsToDesc);
    //after writing the .pomdp file. compile sarsop and solve the problem.
    POMDP_ClosedModel::closedModel.solveModel();
}

#pragma endregion
}

    