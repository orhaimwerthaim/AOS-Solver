
     #include <despot/model_primitives/iros/closed_model_policy.h>
namespace despot { 

vector<double> ClosedModelPolicy::_currentBelief;
map<std::string, map<int,map<int, double>>> ClosedModelPolicy::obsActNState_ObservationModel;//map<observation, map<action,map<nextState, Probability>>>
map<int, map<int, map<int, double>>> ClosedModelPolicy::nextStateActionPrevState_TransitionModel;//map<nextState, map<action, map<prevState, probability>>>
 

vector <std::pair<int, vector<double>>> ClosedModelPolicy::alpha_vectors;

void ClosedModelPolicy::loadAlphaVectorsFromPolicyFile()
{
    if(alpha_vectors.size() > 0)
        return;
    char tmp[256];
    getcwd(tmp, 256);
    std::string workingDirPath(tmp);
    workingDirPath = workingDirPath.substr(0, workingDirPath.find("build"));

    std::string policyFilePath(workingDirPath);
    policyFilePath.append(Globals::config.fixedPolicyFilePath);
    std::ifstream pf(policyFilePath);
    std::string policyFileContent((std::istreambuf_iterator<char>(pf)),
                                  (std::istreambuf_iterator<char>()));

    std::string t = "<Vector ";
    int vecInd = 0;
    std::string actStr = "action=\"";

    //get all alpha vectors
    while (true)
    {
        vecInd = policyFileContent.find(t, vecInd) + t.size();
        if (vecInd < t.size())
        {
            break;
        }

        int actionNum = -1;
        vector<double> alpha_vec;

        //go to action num  "<Vector action="4" obsValue="0"> ..."
        int startActionInd = policyFileContent.find(actStr, vecInd) + actStr.size();
        int endActionInd = policyFileContent.find("\"", startActionInd) + actStr.size();

        stringstream actionNumS;
        actionNum = stoi(policyFileContent.substr(startActionInd, endActionInd - startActionInd));

        vecInd = policyFileContent.find(">", vecInd) + 1;

        //get vector values
        while (true)
        {

            //remove spaces
            while (policyFileContent[vecInd] == ' ')
                vecInd++;
            if (policyFileContent[vecInd] == '<')
            {
                break;
            }
            int endValInd = policyFileContent.find(" ", vecInd);
            double val = std::stod(policyFileContent.substr(vecInd, endValInd - vecInd));
            alpha_vec.push_back(val);
            vecInd = endValInd;
        }

        std::pair<int, vector<double>> vec{actionNum, alpha_vec};
        ClosedModelPolicy::alpha_vectors.push_back(vec);
		} 
}

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
int ClosedModelPolicy::getBestAction()
{
    double maxValue= -DBL_MAX;
    int maxAction = -1;
    for (auto alphaVec : alpha_vectors)
    {
        double value = std::inner_product(_currentBelief.begin(), _currentBelief.end(), alphaVec.second.begin(), 0);
        if(value > maxValue)
        {
            maxValue = value;
            maxAction = alphaVec.first;
        }
    }
    return maxAction;
}



}