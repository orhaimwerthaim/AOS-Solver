
     #include <despot/model_primitives/collect_toys/closed_model_policy.h>
namespace despot { 

vector<double> ClosedModelPolicy::_currentBelief;
map<std::string, map<int,map<int, double>>> ClosedModelPolicy::obsActNState_ObservationModel;//map<observation, map<action,map<nextState, Probability>>>
map<int, map<int, map<int, double>>> ClosedModelPolicy::nextStateActionPrevState_TransitionModel;//map<nextState, map<action, map<prevState, probability>>>
 

vector <std::pair<int, vector<double>>> ClosedModelPolicy::alpha_vectors;


void ClosedModelPolicy::loadInitialBeliefStateFromVector(vector<double> bs)
{
    _currentBelief.clear();
    for (int i = 0; i < bs.size(); i++)
    {
        _currentBelief.push_back(bs[i]);
    }
}

void ClosedModelPolicy::updateBelief(string observation, int action)
{
    vector<double> prev_belief;
    for (int i = 0; i < _currentBelief.size();i++)
    {
        prev_belief.push_back(_currentBelief[i]);
    }

    for (int i = 0; i < _currentBelief.size(); i++)
    {
        double go_to_state_prob=0;
        for (int j = 0; j < prev_belief.size(); j++)
        {
            double tran_prob = nextStateActionPrevState_TransitionModel[i][action][j];
            double b_prev_s = prev_belief[j];
            go_to_state_prob += b_prev_s * tran_prob;
        }

        double prob_to_see_observation_in_state = obsActNState_ObservationModel[observation][action].find(i) != obsActNState_ObservationModel[observation][action].end()
            ? obsActNState_ObservationModel[observation][action][i]
            : 
                (obsActNState_ObservationModel[observation][action].find(STATE_WILD_CARD) != obsActNState_ObservationModel[observation][action].end()
                ? obsActNState_ObservationModel[observation][action][STATE_WILD_CARD] : 0.0);
         

        _currentBelief[i] = prob_to_see_observation_in_state * go_to_state_prob;
    }
}

}