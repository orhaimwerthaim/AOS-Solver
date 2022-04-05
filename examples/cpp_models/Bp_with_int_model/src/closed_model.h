
    #ifndef CLOSED_MODEL_H
#define CLOSED_MODEL_H

#ifndef	NULL
#define NULL 0
#endif
#include "Bp_with_int_model.h"
#include <vector>
#include <map>
#include <set>
#include <string> 
#include <unistd.h>
#include <despot/core/globals.h>
#include <despot/model_primitives/Bp_with_int_model/actionManager.h> 
#include <float.h>
#include <iomanip>
#include <despot/core/pomdp.h> 
using namespace std;
namespace despot {
    class POMDP_ClosedModelState
    {
    public:
        int state;

        //reward model
        bool hasSpecialStateReward = false;
        double specialStateReward;//reward for reaching this state
        map<int, double> actionPreconditionReward;//penalty for starting specific action from this state
        map<std::pair<int,int>,double> actionPrevStateReward;//action cost starting from a specific state and reaching this state
        
       
        //observation model
        map<std::pair<int,int>, double> actionObservationProb;
        
        //transition model
        map<std::pair<int,int>, double> actionNextStateProb;
        bool isTerminalState = false;

        void setAsTerminal();
        void addObservationTransitionRewardData(int _state, int _nextState, int _action, int _observation, double _reward, bool _isNextStateTerminal, double precondition_reward, double _specialStateReward);
        void addObservationAsNextStateSample(int _nextState, int _obs, int _action);
        void addNextStateTransition(int _state, int next_state, int _action);
        void calcModelBySamples();
        POMDP_ClosedModelState();

    private:
        map<int,int> total_samplesFromStateByAction;
        map<int,int> total_samplesToStateByAction;
        map<std::pair<int,int>, int> actionObservationCount;//when arriving to this state by action and observation
        map<std::pair<int,int>, int> actionNextStateCount;//when arriving to next state by action
    };

    class POMDP_ClosedModel
    {
    public:
        static POMDP_ClosedModel closedModel;
        int initialBStateSamplesCount;

        map<int, int> initialBStateParticle;
        map<int, double> stateInitialStateSampleProb;
        map<int, POMDP_ClosedModelState> statesModel;
        map<int, std::string> statesToPomdpFileStates;
        POMDP_ClosedModel();

        void addInitialBStateSample(int _state);
        void addSample(int state, int nextState, int action, int observation, double reward, bool isNextStateTerminal, double precondition_reward, double specialStateReward);
        POMDP_ClosedModelState *getStateModel(int state);
        void calcModelBySamples();
        void solveModel();
        void GenerateModelFile(std::set<int> states, std::map<int, std::string> actionsToDesc,
                                                  std::set<int> observations, std::map<int, std::string> observationsToDesc);
        void CreateAndSolveModel() const;
    };

    class AlphaVectorPolicy
    {
    private:
        static bool isInit;
        static vector<double> currentBelief;
        static map<int,vector<vector<double>>> alpha_vectors;//for each action all of its alpha vectors.

    public:
        static void Init();
    };
}
#endif //ALPHA_VECTOR_POLICY_H
    