
    #ifndef CLOSED_MODEL_POLICY_H
#define CLOSED_MODEL_POLICY_H
const int STATE_WILD_CARD = -1;

#include <vector>
#include <string>
#include <map>  
#include <float.h>
#include <numeric>
#include <unistd.h>
#include <despot/core/globals.h>
#include <iostream>
#include <tuple>
using namespace std;
namespace despot
{

class ClosedModelPolicy {
	private:
        static vector<double> _currentBelief;
        static vector <std::pair<int, vector<double>>> alpha_vectors;

    public: 
        static void loadInitialBeliefStateFromVector(vector<double> bs);
        static void updateBelief(string observation, int action); 
                                                     //when the rule is true for any state
        static map<std::string, map<int, map<int, double>>> obsActNState_ObservationModel; //map<observation, map<action,map<nextState, Probability>>>

        static map<int, map<int, map<int, double>>> nextStateActionPrevState_TransitionModel; //map<nextState, map<action, map<prevState, probability>>>
};
} // namespace despot
#endif //CLOSED_MODEL_POLICY_H